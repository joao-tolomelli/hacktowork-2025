import datetime
from ..repositories import banho_repository
from ..models.banho import Banho
from .. import socketio


class BanhoService:
    @staticmethod
    def get_banhos() -> list[dict]:
        return banho_repository.get_all_banhos()

    @staticmethod
    def get_banho_by_id(banho_id) -> dict | None:
        banho = banho_repository.get_banho_by_id(banho_id)
        if banho:
            return banho.to_dict()
        return None

    @staticmethod
    def notify_banho_update() -> None:
        print("Notifying clients about banho update...")
        socketio.emit('banho_update', {})
        print("Clients notified.")

    @staticmethod
    def set_default_limit(limite: float) -> None:
        banho_repository.update_default_limit(limite)

    @staticmethod
    def set_banho_limit(banho_id, limite: float) -> None:
        banho_repository.update_banho_limit(banho_id, limite)

    @classmethod
    def store_telemetria(cls, data: str) -> None:
        last = banho_repository.get_latest_banho()
        if last is None:
            print("No existing banho record found. Creating a new one...")
            banho = Banho.default()
            banho.volume_agua += float(data)
            banho.duracao += 5  # assumindo que a telemetria chega a cada 5 segundos
            banho_repository.create_banho(banho)
            banho_repository.update_volume_agua(banho._id, banho.volume_agua)
            banho_repository.update_duracao(banho._id, banho.duracao)
            print("New banho record created with initial telemetria data.")
            return banho
        elif last.em_andamento:
            print("Updating existing banho record with new telemetria data...")
            last_banho = Banho(
                _id=last._id,
                data_criacao=last.data_criacao,
                data_modificacao=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                duracao=last.duracao + 5,  # assumindo que a telemetria chega a cada 5 segundos
                volume_agua=last.volume_agua + float(data),
                limite=last.limite,
                em_andamento=last.em_andamento
            )
        else:
            print("Last banho record is not in progress. Creating a new one...")
            last_banho = banho_repository.create_banho()
        banho_repository.update_duracao(last_banho._id, last_banho.duracao)
        banho_repository.update_volume_agua(last_banho._id, last_banho.volume_agua)
        print("Banho record updated successfully.")
        print(last_banho.to_dict())
        if last_banho.volume_agua >= last_banho.limite:
            print("Banho duration limit reached. Sending notification...")
            MessageService.notify_limite("ESTOURO")
            print("Notification sent.")
        elif last_banho.volume_agua >= last_banho.limite - 10.0:
            print("Banho duration limit approaching. Sending notification...")
            MessageService.notify_limite("PROXIMO")
            print("Notification sent.")
        cls.notify_banho_update()
        return last_banho

    @classmethod
    def store_status(cls, data: str) -> None:
        print("Storing status:", data)
        if data == "INICIO":
            print("Starting new banho record...")
            last = banho_repository.get_latest_banho()
            if last and last.em_andamento:
                raise Exception("Já existe um banho em andamento.")
            print("Creating new banho record...")
            banho_repository.create_banho()
        elif data == "FIM":
            print("Finalizing banho record...")
            last = banho_repository.get_latest_banho()
            if last and last.em_andamento:
                print("Updating banho record to finalize...")
                banho_repository.finalize_banho(last._id)
        cls.notify_banho_update()

from .message import MessageService
