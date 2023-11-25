let { execFile } = require("child_process");
let { writeFile } = require("fs");
let { promisify } = require("util");
let vscode = require("vscode");
let net = require("net");
let writeFileAsync = promisify(writeFile);

const serverAddress = "localhost";
const serverPort = 17338;
const client = new net.Socket();

let connectedToServer = false;
let dontReconnect = false;
let pythonChildProcess = null;

function reconnectToServer(attempt=0) {
    client.connect(serverPort, serverAddress, () => {
        connectedToServer = true;
        vscodeButton.text = "$(megaphone) Stop";
        vscodeButton.tooltip = "Stop the spazlint server";

        vscode.window.showInformationMessage("Connected to spazlint!");
    });
    
    client.on("close", () => {
        if(attempt == 3) {
            return vscode.window.showInformationMessage("Attempt #3 to connect to spazlint has failed!");
        }

        if(dontReconnect) {
            dontReconnect = false;
            return;
        }

        console.log("Connection was closed!");

        // if(!connectedToServer) {
        //     vscode.window.showInformationMessage("Attempting to reconnect to spazlint...");
            
        //     pythonChildProcess = execFile("python", [
        //         process.env["SPAZLINT_DIR"] + "\\main.py"
        //     ], (error, stdout, stderr) => {
        //         if (error) {
        //             // this is when the process gets killed
        //             if(error.message.startsWith("Command failed: python ")) {
        //                 vscode.window.showInformationMessage("Spazlint server has successfully stopped!");
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
        //     });
        
        //     setTimeout(() => {
    //         reconnectToServer(attempt + 1);
    //     }, 3_000);
    // } else {
        //     vscode.window.showInformationMessage("Disconnected from spazlint!");
        // }
        
        connectedToServer = false;
    });
}

function stopServer() {
    if(pythonChildProcess != null) {
        dontReconnect = true;
        pythonChildProcess.kill();
        vscodeButton.text = "$(megaphone) Connect";
        vscodeButton.tooltip = "Connect to spazlint!";
    } else vscode.window.showErrorMessage("Attempted to stop server when python process has already been killed!")
}

var extensionDiagnostics;

function getDefaultOf(type) {
    if (["int", "uint", "int8", "uint8", "int16", "uint16", "int64", "uint64", "float", "double"].includes(type))
        return "0";
    if (type == "bool")
        return "false";
    if (type.startsWith("array<"))
        return "array<" + type.split("array<")[1].split(">")[0] + ">() = {}";
    if (type == "string")
        return "\"\"";

    return "undefined";
}

function getFileLocation() {
    var location = vscode.window.activeTextEditor.document.uri.fsPath

    while(location.includes("\\"))
        location = location.replace("\\", "_")
    location = location.replace(":", "_")
    
    return process.env["SPAZLINT_DIR"] + "\\temp\\" + location + ".tmp";
}

async function runPythonScript(adv = false) {
    return new Promise((resolve, reject) => {
        const requestData = {
            type: adv === true ? "ADVANCED_AUTOCOMPLETE" : "AUTOCOMPLETE",
            line: vscode.window.activeTextEditor.selection.active.line.toString(),
            char: vscode.window.activeTextEditor.selection.active.character.toString(),
            file: getFileLocation()
        };

        console.log("Running script!");

        let accumulatedData = "";

        const onDataReceived = (data) => {
            console.log(`chunkRecv, data: ${typeof(data)}; dataStringLength: ${data.toString().length}, accum: ${accumulatedData.length}`);
            accumulatedData += data.toString();

            try {
                JSON.parse(accumulatedData.split("\n")[0]);
                JSON.parse(accumulatedData.split("\n")[1]);
                console.log("Full recv!");
                resolve(accumulatedData);
                accumulatedData = "";
                client.off("data", onDataReceived);
            } catch (error) {
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
                let x = new vscode.CompletionItem(
                    suggestion["name"],
                    suggestion["type"] == "function" ?
                        vscode.CompletionItemKind.Function : (suggestion["type"] == "property" ? vscode.CompletionItemKind.Property : vscode.CompletionItemKind.Variable)
                );

                x.documentation = suggestion["description"] + (suggestion["type"] == "function" ? ("\n\n" + suggestion["full"]) : "");

                if (suggestion["type"] == "function") {
                    let str = suggestion["name"] + "(";
                    let ss = new vscode.SnippetString(str);

                    if (suggestion["items"] != undefined) {
                        var i = 0;

                        suggestion["items"].map(x => {
                            if (x.items != undefined && x.items.length != 0) {
                                let newItems = [];

                                x.items.forEach(itm => {
                                    newItems.push((x.type.split("::")[0] == "jjBEHAVIOR" ? "BEHAVIOR" : x.type.split("::")[0]) + "::" + itm);
                                });

                                ss.appendText(x.name + ": ");
                                ss.appendChoice(newItems);
                            } else {
                                ss.appendText(x.name + ": ");
                                if (!x.defaultValue && getDefaultOf(x.type) == "\"\"") {
                                    ss.appendText("\"");
                                    ss.appendPlaceholder("");
                                    ss.appendText("\"");
                                } else
                                    ss.appendPlaceholder(x.defaultValue || getDefaultOf(x.type));
                            }

                            ss.appendText(i == suggestion["items"].length - 1 ? "" : ", ");
                            i += 1;
                        });
                    }

                    ss.appendText(");");

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
}

async function refreshDiagnostics(advanced=false) {
    if((vscode.window.activeTextEditor == null || vscode.window.activeTextEditor.document.isUntitled || !(vscode.window.activeTextEditor.document.fileName.endsWith(".j2as") || vscode.window.activeTextEditor.document.fileName.endsWith(".mut") || vscode.window.activeTextEditor.document.fileName.endsWith(".asc"))))
        return;
        
    await writeFileAsync(getFileLocation(), vscode.window.activeTextEditor.document.getText());

    try {
        const output = await runPythonScript(advanced);
        const errors = JSON.parse(output.split("\n")[1]);

        var diagnostics = errors.map((diagnostic) => new vscode.Diagnostic(new vscode.Range(diagnostic["line"], diagnostic["char"], diagnostic["line"] + 1, 0), diagnostic["text"], diagnostic["type"] == "ERR" ? vscode.DiagnosticSeverity.Error : (diagnostic["type"] == "INFO" ? vscode.DiagnosticSeverity.Information : vscode.DiagnosticSeverity.Warning)));

        extensionDiagnostics.set(vscode.window.activeTextEditor.document.uri, diagnostics)
    } catch (error) {
        console.error("Error running error linter:", error);
        return [];
    }
}

var vscodeButton;

function activate(context) {
    extensionDiagnostics = vscode.languages.createDiagnosticCollection("jj2plus");
    context.subscriptions.push(extensionDiagnostics);

    context.subscriptions.push(vscode.workspace.onDidChangeTextDocument(e => refreshDiagnostics()));
    context.subscriptions.push(vscode.workspace.onDidSaveTextDocument(e => refreshDiagnostics(true)));
    
    vscodeButton = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 101);
    vscodeButton.text = "$(megaphone) Connect";
    vscodeButton.command = "spazlint.connect";
    vscodeButton.tooltip = "Connect to spazlint!";
    vscodeButton.show();

    context.subscriptions.push(
        vscode.commands.registerCommand("spazlint.connect", () => {
            if(!connectedToServer) {
                reconnectToServer();
            } else {
                stopServer();
            }
        })
    );

    vscode.languages.registerCompletionItemProvider("cpp", completion);

    console.log("[SpazLint] Registered completion items!")
}

function deactivate() { }

module.exports = { activate, deactivate }
