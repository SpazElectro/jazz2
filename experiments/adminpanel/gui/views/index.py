import flet as ft
from flet_route import Params, Basket
from general import getIsConnected

def IndexView(page: ft.Page, params: Params, basket: Basket):
    return ft.View(
        "/",
        controls=[
            ft.Text("Home page"),
            ft.Text(f"Connected: {getIsConnected()}", key="isConnected"),
            ft.ElevatedButton("Go to chat", on_click=lambda _: page.go("/chat"), disabled=False),
            ft.ElevatedButton("Go to players", on_click=lambda _: page.go("/players"), disabled=False),
        ]
    )