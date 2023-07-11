import flask

app = flask.Flask(__name__, static_url_path="/public")

@app.get("/")
def index():
    return flask.send_file("./public/plus-angelscript.html")

@app.get("/scrape")
def scraper():
    return flask.send_file("./scrape.js")

# unsafe but gets the job done
@app.route('/<path:path>')
def static_file(path):
    return flask.send_file("./public/" + path)

app.run("0.0.0.0", 8080, True)