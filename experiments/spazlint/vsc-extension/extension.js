// let { execFile } = require("child_process");
let { writeFile, readFileSync, existsSync } = require("fs");
let { promisify } = require("util");
let vscode = require("vscode");
let net = require("net");
let writeFileAsync = promisify(writeFile);

const serverAddress = "localhost";
const serverPort = 17338;
var client = new net.Socket();

let connectedToServer = false;
let dontReconnect = false;

// let pythonChildProcess = null;

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

function reconnectToServer(attempt = 0) {
    client.connect(serverPort, serverAddress, () => {
        connectedToServer = true;
        vscodeButton.text = "$(megaphone) Stop";
        vscodeButton.tooltip = "Stop the spazlint server";

        vscode.window.showInformationMessage("Connected to spazlint!");
    });

    client.on("close", () => {
        if (attempt == 3) {
            return vscode.window.showInformationMessage(
                "Attempt #3 to connect to spazlint has failed!"
            );
        }

        if (dontReconnect) {
            dontReconnect = false;
            return;
        }

        console.log("Connection was closed!");

        if (!connectedToServer) {
            vscode.window.showInformationMessage(
                "Attempting to reconnect to spazlint..."
            );

            // pythonChildProcess = execFile(
            //     "python",
            //     [vscode.workspace.getConfiguration("spazlint").get("linterdir") + "\\main.py"],
            //     (error, stdout, stderr) => {
            //         if (error) {
            //             // this is when the process gets killed
            //             if (
            //                 error.message.startsWith("Command failed: python ")
            //             ) {
            //                 vscode.window.showInformationMessage(
            //                     "Spazlint server has successfully stopped!"
            //                 );
            //                 return;
            //             } else {
            //                 vscode.window.showErrorMessage(error.message);
            //             }

            //             reject(error.message);
            //         } else if (stderr) {
            //             vscode.window.showErrorMessage(stderr);
            //             reject(stderr);
            //         } else {
            //             resolve(stdout);
            //         }
            //     }
            // );

            setTimeout(() => {
                reconnectToServer(attempt + 1);
            }, 3_000);
        } else {
            vscode.window.showInformationMessage("Disconnected from spazlint!");
            vscodeButton.text = "$(megaphone) Connect";
            vscodeButton.tooltip = "Connect to spazlint!";
        }

        connectedToServer = false;
    });
}

function stopServer() {
    client.destroy();

    // if (pythonChildProcess != null) {
    dontReconnect = true;
        // pythonChildProcess.kill();
    vscodeButton.text = "$(megaphone) Connect";
    vscodeButton.tooltip = "Connect to spazlint!";
    // } else
    //     vscode.window.showErrorMessage(
    //         "Attempted to stop server when python process has already been killed!"
    //     );
}

var extensionDiagnostics;

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

    return "undefined";
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
            line: vscode.window.activeTextEditor.selection.active.line.toString(),
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

async function refreshDiagnostics(advanced = false) {
    if (
        vscode.window.activeTextEditor == null ||
        vscode.window.activeTextEditor.document.isUntitled ||
        !(
            vscode.window.activeTextEditor.document.fileName.endsWith(
                ".j2as"
            ) ||
            vscode.window.activeTextEditor.document.fileName.endsWith(".mut") ||
            vscode.window.activeTextEditor.document.fileName.endsWith(".asc")
        )
    )
        return;
            
    var diagnostics = [];

    // TODO high priority, make includes at the start of the file
    // TODO and set line to the correct line
    let windowText = vscode.window.activeTextEditor.document.getText();
    let filesToInclude = [];
    windowText.split("\n").forEach(line => {
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
            diagnostics.push(new vscode.Diagnostic(
                new vscode.Range(0, 0, 0, 0), `Include ${fileName} was not found! searched at: ${fileContent.join(" and ")}`, vscode.DiagnosticSeverity.Warning
            ));
        } else {
            includesText += fileContent;
        }
    });

    let text = includesText + "\n\n" + windowText;

    await writeFileAsync(
        getFileLocation(),
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
        console.error("Error running error linter:", error);
        return [];
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
                stopServer();
            }
        })
    );

    vscode.languages.registerCompletionItemProvider("cpp", completion);

    console.log("[SpazLint] Registered completion items!");
}

function deactivate() { }

module.exports = { activate, deactivate };
