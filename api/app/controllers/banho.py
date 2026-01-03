from flask import Blueprint, request
from .. import r
from ..repositories import banho_repository
from ..services.banho import BanhoService


bp = Blueprint('banho', __name__)


@bp.route('/', methods=['GET'])
def get_banhos():
    banhos = BanhoService.get_banhos()
    return banhos

@bp.route('/latest', methods=['GET'])
def get_latest_banho():
    banho = banho_repository.get_latest_banho()
    if banho:
        return banho.to_dict()
    return {'message': 'No banho found'}, 404

@bp.route('/limit', methods=['POST'])
def set_banho_limit():
    data = request.json
    limite = data.get('limite')
    if limite is None:
        return {'message': 'Limite is required'}, 400
    BanhoService.set_default_limit(limite)
    return {'message': 'Limite updated successfully'}, 200

@bp.route('/<banho_id>', methods=['GET'])
def get_banho_by_id(banho_id):
    banho = BanhoService.get_banho_by_id(banho_id)
    if banho:
        return banho
    return {'message': 'Banho not found'}, 404

@bp.route('/<banho_id>/limit', methods=['PUT'])
def set_banho_id_limit(banho_id):
    data = request.json
    limite = data.get('limite')
    if limite is None:
        return {'message': 'Limite is required'}, 400
    BanhoService.set_banho_limit(banho_id, limite)
    return {'message': 'Limite updated successfully'}, 200
