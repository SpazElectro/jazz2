import flet as ft
from flet_route import Params, Basket

def IndexView(page: ft.Page, params: Params, basket: Basket):
    # TODO: disable when not connected yet
    
    return ft.View(
        "/",
        controls=[
            ft.Text("Home page"),
            ft.ElevatedButton("Go to chat", on_click=lambda _: page.go("/chat")),
            ft.ElevatedButton("Go to players", on_click=lambda _: page.go("/players")),
        ]
    )