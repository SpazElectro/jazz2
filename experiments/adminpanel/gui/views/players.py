import flet as ft
from flet_route import Params, Basket
from general import websocket_send, assemble_packet
import json, base64

gpage = None
playersList = None

class Player(ft.Row):
    def __init__(self, name: str, char: str, fur: str, roasts: int, deaths: int, team: int):
        super().__init__()

        self.name = name
        self.char = char
        self.fur = fur

        furA = fur[0]
        furB = fur[1]
        furC = fur[2]
        furD = fur[3]

        self.vertical_alignment = "start"
        self.controls = [
            ft.Row([
                ft.Image(src=f"https://jj2multiplayer.online/fur/?a={furA}&b={furB}&c={furC}&d={furD}&char={char}&frame=3"),
                ft.Text(name, selectable=True),
            ], tight=True, spacing=5)
        ]

import hexdump
def on_player(content):
    global playersList, gpage
    players = json.loads(content)

    for data in players:
        print(json.dumps(data, indent=2))

        playersList.controls.append(Player(data["name"], data["character"], base64.b64decode(data["fur"][0]), data["roasts"], data["deaths"], data["team"]))

    gpage.update()

def PlayersView(page: ft.Page, params: Params, basket: Basket):
    global playersList, gpage
    gpage = page
    playersList = ft.ListView(expand=True, spacing=10, auto_scroll=True)
    websocket_send(assemble_packet("request", "players"))

    return ft.View("/players", [
        ft.AppBar(leading=ft.Icon(ft.icons.PALETTE),
            leading_width=40,
            title=ft.Text("Player list"),
            center_title=False,
            bgcolor=ft.colors.SURFACE_VARIANT,
            actions=[
                ft.IconButton(
                    icon=ft.icons.EXIT_TO_APP_ROUNDED,
                    tooltip="Exit",
                    on_click=lambda _: page.go("/"),
                ),
            ],
        ),
        ft.Container(
            content=playersList,
            border=ft.border.all(1, ft.colors.OUTLINE),
            border_radius=5,
            padding=10,
            expand=True,
        ),
    ])