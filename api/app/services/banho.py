from ..repositories import banho_repository


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
    def set_default_limit(limite: float) -> None:
        banho_repository.update_default_limit(limite)

    @staticmethod
    def set_banho_limit(banho_id, limite: float) -> None:
        banho_repository.update_banho_limit(banho_id, limite)
