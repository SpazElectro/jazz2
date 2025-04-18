let { writeFile, readFileSync, existsSync, mkdirSync } = require("fs");
let { promisify } = require("util");
let vscode = require("vscode");
let net = require("net");
const { dirname } = require("path");
let writeFileAsync = promisify(writeFile);

const serverAddress = "localhost";
const serverPort = 17338;
const client = new net.Socket();

let connectedToServer = false;
let extensionDiagnostics;

function stripText(inputString) {
    let startIndex = 0;
    while (startIndex < inputString.length && inputString[startIndex].trim() === "") {
        startIndex++;
    }

    let endIndex = inputString.length - 1;
    while (endIndex >= 0 && inputString[endIndex].trim() === "") {
        endIndex--;
    }

    return inputString.substring(startIndex, endIndex + 1);
}

function reconnectToServer() {
    client.connect(serverPort, serverAddress, () => {
        connectedToServer = true;
        vscodeButton.text = "$(megaphone) Disconnect";
        vscodeButton.tooltip = "Disconnect from the spazlint server";

        vscode.window.showInformationMessage("Connected to spazlint!");
    });

    client.on("close", () => {
        connectedToServer = false;
        vscodeButton.text = "$(megaphone) Connect";
        vscodeButton.tooltip = "Connect to spazlint!";
        
        vscode.window.showInformationMessage("Disconnected from spazlint!");
    });
}

function disconnectFromServer() {
    client.destroy();

    vscodeButton.text = "$(megaphone) Connect";
    vscodeButton.tooltip = "Connect to spazlint!";
}

function getDefaultOf(type) {
    if (
        [
            "int",
            "uint",
            "int8",
            "uint8",
            "int16",
            "uint16",
            "int64",
            "uint64",
            "float",
            "double",
        ].includes(type)
    )
        return "0";
    if (type == "bool") return "false";
    if (type.startsWith("array<"))
        return "array<" + type.split("array<")[1].split(">")[0] + ">() = {}";
    if (type == "string") return '""';
    if (type == "jjPLAYER@") return "jjPlayers[0]";

    return "";
}

function getFileLocation() {
    var location = vscode.window.activeTextEditor.document.uri.fsPath;

    while (location.includes("\\")) location = location.replace("\\", "_");
    location = location.replace(":", "_");

    return vscode.workspace.getConfiguration("spazlint").get("linterdir") + "\\temp\\" + location + ".tmp";
}

async function runPythonScript(adv = false) {
    return new Promise((resolve, reject) => {
        const requestData = {
            type: adv === true ? "ADVANCED_AUTOCOMPLETE" : "AUTOCOMPLETE",
            line: (getIncludesText()[1] + vscode.window.activeTextEditor.selection.active.line).toString(),
            char: vscode.window.activeTextEditor.selection.active.character.toString(),
            file: getFileLocation(),
        };

        let accumulatedData = "";

        const onDataReceived = (data) => {
            accumulatedData += data.toString();

            try {
                JSON.parse(accumulatedData.split("\n")[0]);
                JSON.parse(accumulatedData.split("\n")[1]);
                resolve(accumulatedData);
                console.log("Gathered full data!");
                accumulatedData = "";
                client.off("data", onDataReceived);
            } catch (error) {
                if (!(error instanceof SyntaxError))
                    console.log(`ERR: ${error}`);
            }
        };

        client.on("data", onDataReceived);
        client.write(JSON.stringify(requestData));
    });
}

let completion = {
    async provideCompletionItems() {
        if(!connectedToServer) {
            vscode.window.showInformationMessage("You are not connected to Spazlint!");
            return [];
        }

        try {
            const output = await runPythonScript();
            const suggestions = JSON.parse(output.split("\n")[0]);

            var completionItems = suggestions.map((suggestion) => {
                let isEvent = suggestion["type"] == "event";
                let x = new vscode.CompletionItem(
                    suggestion["name"],
                    suggestion["type"] == "function" || isEvent
                        ? vscode.CompletionItemKind.Function
                        : suggestion["type"] == "property"
                            ? vscode.CompletionItemKind.Property
                            : vscode.CompletionItemKind.Variable
                );

                x.documentation =
                    suggestion["description"] +
                    (suggestion["type"] == "function" || isEvent
                        ? "\n\n" + suggestion["full"]
                        : "");

                if (suggestion["type"] == "function" || isEvent) {
                    let str = suggestion["name"] + "(";
                    let ss = new vscode.SnippetString(str);

                    if (suggestion["items"] != undefined) {
                        var i = 0;

                        suggestion["items"].map((x) => {
                            if (x.items != undefined && x.items.length != 0) {
                                if (isEvent) {
                                    ss.appendText(x.type + " " + x.name);
                                } else {
                                    let newItems = [];

                                    x.items.forEach((itm) => {
                                        newItems.push(
                                            (x.type.split("::")[0] ==
                                                "jjBEHAVIOR"
                                                ? "BEHAVIOR"
                                                : x.type.split("::")[0]) +
                                            "::" +
                                            itm
                                        );
                                    });

                                    ss.appendText(x.name + ": ");
                                    ss.appendChoice(newItems);
                                }
                            } else {
                                if (isEvent) {
                                    ss.appendText(x.type + " " + x.name);
                                } else {
                                    ss.appendText(x.name + ": ");

                                    if (
                                        !x.defaultValue &&
                                        getDefaultOf(x.type) == '""'
                                    ) {
                                        ss.appendText('"');
                                        ss.appendPlaceholder("");
                                        ss.appendText('"');
                                    } else {
                                        ss.appendPlaceholder(
                                            x.defaultValue ||
                                            getDefaultOf(x.type)
                                        );
                                    }
                                }
                            }

                            ss.appendText(
                                i == suggestion["items"].length - 1 ? "" : ", "
                            );
                            i += 1;
                        });
                    }

                    ss.appendText(isEvent ? ") {\n\n}" : ");");

                    x.insertText = ss;
                }

                return x;
            });

            return completionItems;
        } catch (error) {
            console.error("Error running linter:", error);
            return [];
        }
    },
};

function findIncludeFile(filename) {
    /**
     * @type {Array<string>}
     */
    let locations = vscode.workspace.getConfiguration("spazlint").get("includedir");
    let lookedAt = [];

    for (let index = 0; index < locations.length; index++) {
        var x = locations[index];
        if(vscode.workspace.workspaceFolders != [])
            x = x.replace("${WORKSPACE_DIR}", vscode.workspace.workspaceFolders[0].uri.fsPath)
        
        let locPath = x+"\\"+filename;
        lookedAt.push(locPath);

        if(existsSync(locPath)) {
            return [true, readFileSync(locPath)];
        }
    }

    return [false, lookedAt];
}

let diagnosticsQueue = [];

function getIncludesText() {
    let filesToInclude = [];
    vscode.window.activeTextEditor.document.getText().split("\n").forEach(line => {
        line = stripText(line);
        if(line.startsWith("#include \"")) {
            let including = line.split("\"")[1].split("\"")[0]
            filesToInclude.push(including);
        }
    });

    let includesText = "";
    
    // find the file
    filesToInclude.forEach(fileName => {
        let [success, fileContent] = findIncludeFile(fileName);
        
        if(success == false) {
            diagnosticsQueue.push(new vscode.Diagnostic(
                new vscode.Range(0, 0, 0, 0), `Include ${fileName} was not found! searched at: ${fileContent.join(" and ")}, You can set the include directories in the settings. (spazlint.includedir)`, vscode.DiagnosticSeverity.Warning
            ));
        } else {
            includesText += fileContent;
        }
    });
    
    return [includesText, includesText.split("\n").length+2]
}

async function refreshDiagnostics(advanced = false) {
    if (vscode.window.activeTextEditor == null || vscode.window.activeTextEditor.document.isUntitled ||
        !(
            vscode.window.activeTextEditor.document.fileName.endsWith(".j2as") ||
            vscode.window.activeTextEditor.document.fileName.endsWith(".mut") ||
            vscode.window.activeTextEditor.document.fileName.endsWith(".asc")
        )
    ) return;
    
    if(vscode.workspace.getConfiguration("spazlint").get("linterdir") == "") {
        return vscode.window.showInformationMessage("Spazlint directory has not been set, set it in the settings! (spazlint.linterdir)");
    }

    if(!connectedToServer) {
        return vscode.window.showInformationMessage("You are not connected to Spazlint!");
    }

    var diagnostics = diagnosticsQueue;

    let windowText = vscode.window.activeTextEditor.document.getText();

    let includesText = getIncludesText()[0];

    let text = includesText + "\n\n" + windowText;
    let fileLoc = getFileLocation();

    if (!existsSync(dirname(fileLoc))){
        mkdirSync(dirname(fileLoc));
    }

    await writeFileAsync(
        fileLoc,
        text
    );

    try {
        const output = await runPythonScript(advanced);
        const errors = JSON.parse(output.split("\n")[1]);

        errors.map(
            (diagnostic) =>
                new vscode.Diagnostic(
                    new vscode.Range(
                        diagnostic["line"],
                        diagnostic["char"],
                        diagnostic["line"] + 1,
                        0
                    ),
                    diagnostic["text"],
                    diagnostic["type"] == "ERR"
                        ? vscode.DiagnosticSeverity.Error
                        : diagnostic["type"] == "INFO"
                            ? vscode.DiagnosticSeverity.Information
                            : vscode.DiagnosticSeverity.Warning
                )
        ).forEach(x => diagnostics.push(x));

        extensionDiagnostics.set(
            vscode.window.activeTextEditor.document.uri,
            diagnostics
        );
    } catch (error) {
        console.error("Error running linter:", error);
        return;
    }
}

var vscodeButton;

function activate(context) {
    extensionDiagnostics =
        vscode.languages.createDiagnosticCollection("jj2plus");
    context.subscriptions.push(extensionDiagnostics);

    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument((e) => refreshDiagnostics())
    );
    context.subscriptions.push(
        vscode.workspace.onDidSaveTextDocument((e) => refreshDiagnostics(true))
    );

    vscodeButton = vscode.window.createStatusBarItem(
        vscode.StatusBarAlignment.Right,
        101
    );
    vscodeButton.command = "spazlint.connect";
    vscodeButton.text = "$(megaphone) Connect";
    vscodeButton.tooltip = "Connect to spazlint!";
    vscodeButton.show();

    context.subscriptions.push(
        vscode.commands.registerCommand("spazlint.connect", () => {
            if (!connectedToServer) {
                reconnectToServer();
            } else {
                disconnectFromServer();
            }
        })
    );

    vscode.languages.registerCompletionItemProvider("cpp", completion);

    console.log("[SpazLint] Registered completion items!");
}

function deactivate() { }

module.exports = { activate, deactivate };
