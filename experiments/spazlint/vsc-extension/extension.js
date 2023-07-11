let exec = require("child_process").exec;
let vscode = require("vscode");

var _context;
var extensionDiagnostics;

async function runPythonScript() {
    return new Promise((resolve, reject) => {
        exec(
            "python G:\\steve\\jazz2stuff\\jazz2\\experiments\\spazlint\\main.py \""
            + vscode.window.activeTextEditor.document.uri.fsPath + "\" \"" +
            vscode.window.activeTextEditor.document.lineAt(vscode.window.activeTextEditor.selection.active.line).text + "\" \"" +
            vscode.window.activeTextEditor.selection.active.character + "\"", (error, stdout, stderr) => {
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
                                ss.appendChoice(newItems)
                            } else {
                                ss.appendText(x.name + ": ");
                                ss.appendPlaceholder(x.defaultValue || "0");
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

async function refreshDiagnostics() {
    try {
        const output = await runPythonScript();
        const errors = JSON.parse(output.split("\n")[1]);

        var diagnostics = errors.map((diagnostic) => new vscode.Diagnostic(new vscode.Range(diagnostic["line"], 0, diagnostic["line"] + 1, 0), diagnostic["text"]));
        console.log("diagnostics count: " + diagnostics.length);

        extensionDiagnostics.set(vscode.window.activeTextEditor.document.uri, diagnostics)
    } catch (error) {
        console.error("Error running error linter:", error);
        return [];
    }
}

function activate(context) {
    console.log("we running")

    _context = context;
    extensionDiagnostics = vscode.languages.createDiagnosticCollection("jj2plus");
    context.subscriptions.push(extensionDiagnostics);

    context.subscriptions.push(vscode.workspace.onDidChangeTextDocument(e => refreshDiagnostics()))

    vscode.languages.registerCompletionItemProvider("c++", completion);
    vscode.languages.registerCompletionItemProvider("cpp", completion);

    console.log("registered lol")
}

function deactivate() { }

module.exports = { activate, deactivate }
