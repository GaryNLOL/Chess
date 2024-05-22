#include "../../include/model/GameState.hpp"
#include <stdexcept>

GameState::GameState()
    : turn_color(PieceColor::White)
    , board({ new Piece(PieceColor::Black, PieceType::Rook), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Rook),
          new Piece(PieceColor::Black, PieceType::Knight), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Knight),
          new Piece(PieceColor::Black, PieceType::Bishop), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Bishop),
          new Piece(PieceColor::Black, PieceType::Queen), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Queen),
          new Piece(PieceColor::Black, PieceType::King), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::King),
          new Piece(PieceColor::Black, PieceType::Bishop), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Bishop),
          new Piece(PieceColor::Black, PieceType::Knight), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Knight),
          new Piece(PieceColor::Black, PieceType::Rook), new Piece(PieceColor::Black, PieceType::Pawn), nullptr, nullptr, nullptr, nullptr, new Piece(PieceColor::White, PieceType::Pawn), new Piece(PieceColor::White, PieceType::Rook) })
    , pawn_double_moved_last_turn(nullptr)
{
}

Piece*& GameState::accessBoard(short int col, short int row)
{
    if (col > 7 || col < 0 || row > 7 || row < 0) {
        throw std::out_of_range("Out of board's range.");
    }
    return board[col * 8 + row];
}

Piece* GameState::readBoard(short int col, short int row) const
{
    if (col > 7 || col < 0 || row > 7 || row < 0) {
        return nullptr;
    }
    return board[col * 8 + row];
}

bool GameState::existInterrumptions(BoardCoordinate source, BoardCoordinate destiny) const
{
    if (source.getCol() == destiny.getCol()) {
        short int modifier = source.getRow() < destiny.getRow() ? 1 : -1;
        for (short int i = source.getRow(); i != destiny.getRow(); i += modifier)
            if (readBoard(source.getCol(), i) != nullptr)
                return true;
        return false;
    } else if (source.getRow() == destiny.getRow()) {
        short int modifier = source.getCol() < destiny.getCol() ? 1 : -1;
        for (short int i = source.getCol(); i != destiny.getCol(); i += modifier)
            if (board[i * 8 + source.getRow()] != nullptr)
                return true;
        return false;
    } else if (abs(source.getRow() - destiny.getRow()) == abs(source.getCol() - destiny.getCol())) {
        short int modifierX = source.getCol() < destiny.getCol() ? 1 : -1;
        short int modifierY = source.getRow() < destiny.getRow() ? 1 : -1;
        for (short int i = source.getCol(), j = source.getRow(); i != destiny.getCol() || j != destiny.getRow(); i += modifierX, j += modifierY)
            if (board[i * 8 + j] != nullptr)
                return true;
        return false;
    } else {
        return true;
    }
}

PieceColor GameState::getTurnColor() const
{
    return turn_color;
}

void GameState::changeTurnColor()
{
    turn_color = turn_color == PieceColor::White ? PieceColor::Black : PieceColor::White;
}

bool GameState::isLegalMove(const Piece moving, const BoardCoordinate source, const BoardCoordinate destiny) const
{
    // Check that moved piece belongs to current player.
    if (moving.getColor() != getTurnColor()) {
        return false;
    }

    // Check that destiny square does not have a piece belonging to the current player
    if (source == destiny
        || (readBoard(destiny.getCol(), destiny.getRow()) != nullptr
            && moving.getColor() == readBoard(destiny.getCol(), destiny.getRow())->getColor())) {
        return false;
    }

    // Get delta X, delta Y, the position of the current player's king, and the opposing color.
    const short int movedX = abs(source.getCol() - destiny.getCol());
    const short int movedY = abs(source.getRow() - destiny.getRow());
    const BoardCoordinate kingPosition = [&]() {
        for (short int i = 1; i <= 8; i++) {
            for (short int j = 1; j <= 8; j++) {
                BoardCoordinate cur(static_cast<BoardCoordinate::Column>(i), j);
                if (readBoard(cur.getCol(), cur.getRow()) != nullptr && readBoard(cur.getCol(), cur.getRow())->getType() == PieceType::King && readBoard(cur.getCol(), cur.getRow())->getColor() == moving.getColor())
                    return cur;
            }
        }
        return destiny;
    }();
    const PieceColor oposingColor = moving.getColor() == PieceColor::Black ? PieceColor::White : PieceColor::Black;

    // Check that opposing knights will not keep making check after movement.
    for (std::pair<short int, short int> knightPlaces : std::initializer_list<std::pair<short int, short int>> { { -2, -1 }, { -1, -2 }, { -1, 2 }, { 2, -1 }, { 1, 2 }, { 2, 1 }, { 1, -2 }, { -2, 1 } }) {
        if (readBoard(kingPosition.getCol() + knightPlaces.first, kingPosition.getRow() + knightPlaces.second) != nullptr && readBoard(kingPosition.getCol() + knightPlaces.first, kingPosition.getRow() + knightPlaces.second)->getType() == PieceType::Knight && readBoard((kingPosition.getCol() + knightPlaces.first), kingPosition.getRow() + knightPlaces.second)->getColor() == oposingColor && !(destiny.getCol() == kingPosition.getCol() + knightPlaces.first && destiny.getRow() == kingPosition.getRow() + knightPlaces.second)) {
            return false;
        }
    }

    // Check that opposing rooks, bishops and queens will not keep making check after movement.
    for (std::pair<short int, short int> modifiers : std::initializer_list<std::pair<short int, short int>> { { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 }, { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } }) {
        for (int x = kingPosition.getCol() + modifiers.first, y = kingPosition.getRow() + modifiers.second; x < 8 && x >= 0 && y < 8 && y >= 0; x += modifiers.first, y += modifiers.second) {
            if (readBoard(x, y) != nullptr) {
                if (readBoard(x, y)->getColor() == oposingColor
                    && !(destiny.getCol() == x && destiny.getRow() == y)
                    && (readBoard(x, y)->getType() == PieceType::Queen
                        || ((abs(modifiers.first) != abs(modifiers.second) && readBoard(x, y)->getType() == PieceType::Rook)
                            || (abs(modifiers.first) == abs(modifiers.second) && readBoard(x, y)->getType() == PieceType::Bishop)))) {
                    return false;
                } else {
                    break;
                }
            } else if (destiny.getCol() == x && destiny.getRow() == y) {
                break;
            }
        }
    }

    // Check that opposing pawns will not keep making check after movement.
    for (int i : { -1, 1 }) {
        if (readBoard(kingPosition.getCol() + i, kingPosition.getRow() + (oposingColor == PieceColor::White ? 1 : -1)) != nullptr && readBoard(kingPosition.getCol() + i, kingPosition.getRow() + (oposingColor == PieceColor::White ? 1 : -1))->getColor() == oposingColor && readBoard(kingPosition.getCol() + i, kingPosition.getRow() + (oposingColor == PieceColor::White ? 1 : -1))->getType() == PieceType::Pawn) {
            return false;
        }
    }

    // Check that piece can reach destination.
    switch (moving.getType()) {
    case PieceType::King:
        if (existInterrumptions(source, destiny)) {
            return false;
        }
        if (destiny.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::G) && board[BoardCoordinate::columnToInt(BoardCoordinate::Column::H) * 8 + source.getRow()] != nullptr && !board[BoardCoordinate::columnToInt(BoardCoordinate::Column::H) * 8 + source.getRow()]->hasMoved() && movedY == 0 && !moving.hasMoved() && isLegalMove(moving, source, BoardCoordinate(BoardCoordinate::Column::F, moving.getColor() == PieceColor::White ? 1 : 8))) {
            return true;
        }
        if (destiny.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::C) && board[BoardCoordinate::columnToInt(BoardCoordinate::Column::A) * 8 + source.getRow()] != nullptr && !board[BoardCoordinate::columnToInt(BoardCoordinate::Column::A) * 8 + source.getRow()]->hasMoved() && movedY == 0 && !moving.hasMoved() && isLegalMove(moving, source, BoardCoordinate(BoardCoordinate::Column::D, moving.getColor() == PieceColor::White ? 1 : 8))) {
            return true;
        }
        if (movedX > 1 || movedY > 1) {
            return false;
        }
        return true;
    case PieceType::Queen:
        if (existInterrumptions(source, destiny)) {
            return false;
        }
        return true;
    case PieceType::Bishop:
        if (movedX != movedY) {
            return false;
        }
        if (existInterrumptions(source, destiny)) {
            return false;
        }
        return true;
    case PieceType::Knight:
        if (!((movedX == 2 && movedY == 1) || (movedX == 1 && movedY == 2))) {
            return false;
        }
        return true;
    case PieceType::Rook:
        if (movedX != 0 && movedY != 0) {
            return false;
        }
        if (existInterrumptions(source, destiny)) {
            return false;
        }
        return true;
    case PieceType::Pawn:
        if ((moving.getColor() == PieceColor::White && BoardCoordinate::rowToInt(source.getRow()) > BoardCoordinate::rowToInt(destiny.getRow())) || (moving.getColor() == PieceColor::Black && BoardCoordinate::rowToInt(source.getRow()) < BoardCoordinate::rowToInt(destiny.getRow()))) {
            return false;
        }
        if (existInterrumptions(source, destiny)) {
            return false;
        }
        if (((moving.getColor() == PieceColor::White && source.getRow() == BoardCoordinate::rowToInt(2)) || (moving.getColor() == PieceColor::Black && source.getRow() == BoardCoordinate::rowToInt(7)))
            && movedX == 0 && movedY == 2 && this->board[destiny.getCol() * 8 + destiny.getRow()] == nullptr) {
            return true;
        } else if (movedX == 1 && movedY == 1 && (this->board[destiny.getCol() * 8 + destiny.getRow()] != nullptr || (source.getRow() == BoardCoordinate::rowToInt(5) && moving.getColor() == PieceColor::White && this->board[destiny.getCol() * 8 + destiny.getRow() + 1] != nullptr && this->board[destiny.getCol() * 8 + destiny.getRow() + 1]->getType() == PieceType::Pawn && this->board[destiny.getCol() * 8 + destiny.getRow() + 1] == this->pawn_double_moved_last_turn) || (source.getRow() == BoardCoordinate::rowToInt(4) && moving.getColor() == PieceColor::Black && this->board[destiny.getCol() * 8 + destiny.getRow() - 1] != nullptr && this->board[destiny.getCol() * 8 + destiny.getRow() - 1]->getType() == PieceType::Pawn && this->board[destiny.getCol() * 8 + destiny.getRow() - 1] == this->pawn_double_moved_last_turn))) {
            return true;
        } else if (movedX != 0 || movedY != 1)
            return false;
        return true;
    default:
        return false;
    }
}