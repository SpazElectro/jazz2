import flet as ft
from flet_route import Params, Basket
from datetime import datetime
from general import assemble_packet, websocket_send

chat = None
gpage = None

def on_message(message: str):
    global chat
    chat.controls.append(Message(message))
    gpage.update()

def get_current_formatted_time():
    now = datetime.now()
    formatted_time = f"[{now.strftime('%H:%M:%S')}]"
    return formatted_time

class Message(ft.Row):
    def __init__(self, text: str):
        super().__init__()
        self.text = text
        self.vertical_alignment = "start"
        self.controls = [ft.Column([ft.Text(text, selectable=True)], tight=True, spacing=5)]

def ChatView(page: ft.Page, params: Params, basket: Basket):
    global chat, gpage
    gpage = page
    page.horizontal_alignment = "stretch"
    page.title = "Chat"

    def send_message_click(e):
        if new_message.value != "":
            websocket_send(assemble_packet("message", new_message.value))
            if not new_message.value.startswith("/"):
                on_message(f"{get_current_formatted_time()} Admin: {new_message.value}")
            new_message.value = ""
            new_message.focus()
            page.update()

    def message_text_change(e):
        if new_message.value.startswith("/login "):
            new_message.password = True
        elif new_message.password == True:
            new_message.password = False
        page.update()

    # Chat messages
    chat = ft.ListView(
        expand=True,
        spacing=10,
        auto_scroll=True,
    )

    # A new message entry form
    new_message = ft.TextField(
        hint_text="Write a message...",
        autofocus=True,
        shift_enter=True,
        min_lines=1,
        max_lines=1,
        filled=True,
        expand=True,
        max_length=250, # rounded from 251
        on_submit=send_message_click,
        on_change=message_text_change
    )

    # Add everything to the page
    return ft.View("/chat", [
        ft.AppBar(leading=ft.Icon(ft.icons.LIST_ROUNDED),
            leading_width=40,
            title=ft.Text("Chat"),
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
            content=chat,
            border=ft.border.all(1, ft.colors.OUTLINE),
            border_radius=5,
            padding=10,
            expand=True,
        ),
        ft.Row(
            [
                new_message,
                ft.IconButton(
                    icon=ft.icons.SEND_ROUNDED,
                    tooltip="Send message",
                    on_click=send_message_click,
                ),
            ]
        ),
    ])