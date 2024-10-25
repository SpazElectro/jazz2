from flet_route import Routing, path

import flet as ft
import general
import threading

gpage: ft.Page

def onmessage(content):
    if gpage.route == "/chat":
        chat.on_message(content)

def onplayers(content):
    if gpage.route == "/players":
        players.on_player(content)

if __name__ == "__main__":
    from views import chat
    from views import players
    from views import index

    def main(page: ft.Page):
        global gpage
        gpage = page
        threading.Thread(target=general.websocket_loop, args=(onmessage, onplayers, page)).start()

        app_routes = [
            path(url="/", clear=False, view=index.IndexView),
            path(url="/chat", clear=False, view=chat.ChatView),
            path(url="/players", clear=False, view=players.PlayersView),
        ]

        Routing(
            page=page,
            app_routes=app_routes
        )

        page.go(page.route)

    ft.app(target=main)
