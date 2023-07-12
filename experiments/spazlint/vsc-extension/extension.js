let { execFile } = require("child_process");
let { writeFileSync } = require("fs");
let vscode = require("vscode");

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
        execFile("python", [
            process.env["SPAZLINT_DIR"] + "\\main.py",
            // vscode.window.activeTextEditor.document.uri.fsPath,
            getFileLocation(),
            vscode.window.activeTextEditor.document.lineAt(vscode.window.activeTextEditor.selection.active.line).text,
            vscode.window.activeTextEditor.selection.active.character.toString(),
            adv ? "true" : "false"
        ], (error, stdout, stderr) => {
            if (error) {
                reject(error.message);
            } else if (stderr) {
                reject(stderr);
            } else {
                resolve(stdout);
            }
        });
    });
}

let completion = {
    async provideCompletionItems() {
        try {
            const output = await runPythonScript();
            const suggestions = JSON.parse(output.split("\n")[0]);

            var completionItems = suggestions.map((suggestion) => {
                let x = new vscode.CompletionItem(suggestion["name"], suggestion["type"] == "function" ? vscode.CompletionItemKind.Function : vscode.CompletionItemKind.Property);
                x.documentation = suggestion["description"] + (suggestion["type"] == "function" ? ("\n\n" + suggestion["returns"] + " " + suggestion["arguments"]) : "");

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
        
    writeFileSync(getFileLocation(), vscode.window.activeTextEditor.document.getText());

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

function activate(context) {
    extensionDiagnostics = vscode.languages.createDiagnosticCollection("jj2plus");
    context.subscriptions.push(extensionDiagnostics);

    context.subscriptions.push(vscode.workspace.onDidChangeTextDocument(e => refreshDiagnostics()));
    context.subscriptions.push(vscode.workspace.onDidSaveTextDocument(e => refreshDiagnostics(true)));

    vscode.languages.registerCompletionItemProvider("cpp", completion);

    console.log("[SpazLint] Registered completion items!")
}

function deactivate() { }

module.exports = { activate, deactivate }
