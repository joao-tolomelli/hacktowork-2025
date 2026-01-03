from .. import socketio
from ..repositories import banho_repository


class MessageService:
    @staticmethod
    def handle_telemetria(message):
        data = message["data"].decode()
        banho_repository.store_telemetria(data)

    @staticmethod
    def handle_status(message):
        data = message["data"].decode()
        banho_repository.store_status(data)
