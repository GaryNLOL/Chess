#include "../../include/model/GameState.hpp"
#include <stdexcept>

GameState::GameState()
    : m_turn_color(PieceColor::White)
    , m_board({ Piece(PieceColor::Black, PieceType::Rook), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Rook),
          Piece(PieceColor::Black, PieceType::Knight), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Knight),
          Piece(PieceColor::Black, PieceType::Bishop), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Bishop),
          Piece(PieceColor::Black, PieceType::Queen), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Queen),
          Piece(PieceColor::Black, PieceType::King), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::King),
          Piece(PieceColor::Black, PieceType::Bishop), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Bishop),
          Piece(PieceColor::Black, PieceType::Knight), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Knight),
          Piece(PieceColor::Black, PieceType::Rook), Piece(PieceColor::Black, PieceType::Pawn), std::nullopt, std::nullopt, std::nullopt, std::nullopt, Piece(PieceColor::White, PieceType::Pawn), Piece(PieceColor::White, PieceType::Rook) })
    , pawn_double_moved_last_turn(std::nullopt)
{
}

std::optional<Piece>& GameState::accessBoard(short int col, short int row)
{
    if (col > 7 || col < 0 || row > 7 || row < 0) {
        throw std::out_of_range("Out of board's range.");
    }
    return m_board[col * 8 + row];
}

std::optional<Piece> GameState::readBoard(short int col, short int row) const
{
    if (col > 7 || col < 0 || row > 7 || row < 0) {
        return std::nullopt;
    }
    return m_board[col * 8 + row];
}

bool GameState::existInterrumptions(BoardCoordinate source, BoardCoordinate destiny) const
{
    if (source.getCol() == destiny.getCol()) {
        short int modifier = source.getRow() < destiny.getRow() ? 1 : -1;
        for (short int i = source.getRow(); i != destiny.getRow(); i += modifier) {
            if (readBoard(source.getCol(), i).has_value()) {
                return true;
            }
        }
        return false;
    } else if (source.getRow() == destiny.getRow()) {
        short int modifier = source.getCol() < destiny.getCol() ? 1 : -1;
        for (short int i = source.getCol(); i != destiny.getCol(); i += modifier) {
            if (readBoard(i, source.getRow()).has_value()) {
                return true;
            }
        }
        return false;
    } else if (abs(source.getRow() - destiny.getRow()) == abs(source.getCol() - destiny.getCol())) {
        short int modifierX = source.getCol() < destiny.getCol() ? 1 : -1;
        short int modifierY = source.getRow() < destiny.getRow() ? 1 : -1;
        for (short int i = source.getCol(), j = source.getRow(); i != destiny.getCol() || j != destiny.getRow(); i += modifierX, j += modifierY) {
            if (readBoard(i, j).has_value()) {
                return true;
            }
        }
        return false;
    } else {
        return true;
    }
}

PieceColor GameState::getTurnColor() const
{
    return m_turn_color;
}

void GameState::changeTurnColor()
{
    m_turn_color = m_turn_color == PieceColor::White ? PieceColor::Black : PieceColor::White;
}

bool GameState::isLegalMove(const BoardCoordinate source, const BoardCoordinate destiny) const
{
    const Piece moving = *this->readBoard(source.getCol(), source.getRow());
    // Check that moved piece belongs to current player.
    if (moving.getColor() != getTurnColor()) {
        return false;
    }

    // Check that destiny square does not have a piece belonging to the current player
    if (source == destiny
        || (readBoard(destiny.getCol(), destiny.getRow()).has_value()
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
                if (readBoard(cur.getCol(), cur.getRow()).has_value()
                    && readBoard(cur.getCol(), cur.getRow())->getType() == PieceType::King
                    && readBoard(cur.getCol(), cur.getRow())->getColor() == moving.getColor()) {
                    return cur;
                }
            }
        }
        return destiny;
    }();
    const PieceColor oposingColor = moving.getColor() == PieceColor::Black ? PieceColor::White : PieceColor::Black;

    // Check that opposing knights will not keep making check after movement.
    for (std::pair<short int, short int> knightPlaces : std::initializer_list<std::pair<short int, short int>> { { -2, -1 }, { -1, -2 }, { -1, 2 }, { 2, -1 }, { 1, 2 }, { 2, 1 }, { 1, -2 }, { -2, 1 } }) {
        if (readBoard(kingPosition.getCol() + knightPlaces.first, kingPosition.getRow() + knightPlaces.second).has_value()
            && readBoard(kingPosition.getCol() + knightPlaces.first, kingPosition.getRow() + knightPlaces.second)->getType() == PieceType::Knight && readBoard((kingPosition.getCol() + knightPlaces.first), kingPosition.getRow() + knightPlaces.second)->getColor() == oposingColor && !(destiny.getCol() == kingPosition.getCol() + knightPlaces.first && destiny.getRow() == kingPosition.getRow() + knightPlaces.second)) {
            return false;
        }
    }

    // Check that opposing rooks, bishops and queens will not keep making check after movement.
    for (std::pair<short int, short int> modifiers : std::initializer_list<std::pair<short int, short int>> { { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 }, { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } }) {
        for (int x = kingPosition.getCol() + modifiers.first, y = kingPosition.getRow() + modifiers.second; x < 8 && x >= 0 && y < 8 && y >= 0; x += modifiers.first, y += modifiers.second) {
            if (readBoard(x, y).has_value()) {
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
        if (readBoard(kingPosition.getCol() + i, kingPosition.getRow() + (oposingColor == PieceColor::White ? 1 : -1)).has_value()
            && readBoard(kingPosition.getCol() + i, kingPosition.getRow() + (oposingColor == PieceColor::White ? 1 : -1))->getColor() == oposingColor
            && readBoard(kingPosition.getCol() + i, kingPosition.getRow() + (oposingColor == PieceColor::White ? 1 : -1))->getType() == PieceType::Pawn) {
            return false;
        }
    }

    // Check that piece can reach destination.
    switch (moving.getType()) {
    case PieceType::King:
        if (existInterrumptions(source, destiny)) {
            return false;
        }
        if (destiny.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::G)
            && m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::H) * 8 + source.getRow()].has_value() && !m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::H) * 8 + source.getRow()]->hasMoved()
            && movedY == 0
            && !moving.hasMoved()
            && isLegalMove(source, BoardCoordinate(BoardCoordinate::Column::F, moving.getColor() == PieceColor::White ? 1 : 8))) {
            return true;
        }
        if (destiny.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::C)
            && m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::A) * 8 + source.getRow()].has_value()
            && !m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::A) * 8 + source.getRow()]->hasMoved()
            && movedY == 0
            && !moving.hasMoved()
            && isLegalMove(source, BoardCoordinate(BoardCoordinate::Column::D, moving.getColor() == PieceColor::White ? 1 : 8))) {
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
            && movedX == 0 && movedY == 2 && !this->m_board[destiny.getCol() * 8 + destiny.getRow()].has_value()) {
            return true;
        } else if (movedX == 1
            && movedY == 1
            && (this->m_board[destiny.getCol() * 8 + destiny.getRow()].has_value()
                || (source.getRow() == BoardCoordinate::rowToInt(5)
                    && moving.getColor() == PieceColor::White
                    && this->m_board[destiny.getCol() * 8 + destiny.getRow() + 1].has_value()
                    && this->m_board[destiny.getCol() * 8 + destiny.getRow() + 1]->getType() == PieceType::Pawn
                    && this->m_board[destiny.getCol() * 8 + destiny.getRow() + 1] == this->pawn_double_moved_last_turn)
                || (source.getRow() == BoardCoordinate::rowToInt(4)
                    && moving.getColor() == PieceColor::Black
                    && this->m_board[destiny.getCol() * 8 + destiny.getRow() - 1].has_value()
                    && this->m_board[destiny.getCol() * 8 + destiny.getRow() - 1]->getType() == PieceType::Pawn
                    && this->m_board[destiny.getCol() * 8 + destiny.getRow() - 1] == this->pawn_double_moved_last_turn))) {
            return true;
        } else if (movedX != 0 || movedY != 1)
            return false;
        return true;
    default:
        return false;
    }
}

void GameState::move(const BoardCoordinate source, const BoardCoordinate destiny)
{
    std::optional<Piece>& moving_piece = accessBoard(source.getCol(), destiny.getRow());

    // Move piece.
    pawn_double_moved_last_turn.reset();
    moving_piece->setAsMoved();
    if (moving_piece->getType() == PieceType::Pawn
        && source.getCol() != destiny.getCol()
        && !readBoard(destiny.getCol(), destiny.getRow()).has_value()) {
        // Check if move is en passant, and if it is remove the pawn.
        if (moving_piece->getColor() == PieceColor::White
            && readBoard(destiny.getCol(), destiny.getRow() + 1).has_value()) {
            accessBoard(destiny.getCol(), destiny.getRow() + 1).reset();
        } else if (moving_piece->getColor() == PieceColor::Black
            && readBoard(destiny.getCol(), destiny.getRow() - 1).has_value()) {
            accessBoard(destiny.getCol(), destiny.getRow() - 1).reset();
        }
    }
    if (readBoard(destiny.getCol(), destiny.getRow()).has_value())
        // Check if move is a capture, and if it is, remove the targetted piece.
        accessBoard(destiny.getCol(), destiny.getRow()).reset();
    if (moving_piece->getType() == PieceType::King
        && source.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::E)
        && destiny.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::C)) {
        // Check if move is long castling, and if it is, move the rook.
        accessBoard(BoardCoordinate::columnToInt(BoardCoordinate::Column::A), source.getRow())->setAsMoved();
        std::swap(this->m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::A) * 8 + source.getRow()],
            this->m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::D) * 8 + source.getRow()]);
    } else if (moving_piece->getType() == PieceType::King
        && source.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::E)
        && destiny.getCol() == BoardCoordinate::columnToInt(BoardCoordinate::Column::G)) {
        // Check if move is short castling, and if it is, move the rook.
        this->accessBoard(BoardCoordinate::columnToInt(BoardCoordinate::Column::H), source.getRow())->setAsMoved();
        std::swap(this->m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::H) * 8 + source.getRow()], this->m_board[BoardCoordinate::columnToInt(BoardCoordinate::Column::F) * 8 + source.getRow()]);
    } else if (moving_piece->getType() == PieceType::Pawn
        && destiny.getRow() == BoardCoordinate::rowToInt(moving_piece->getColor() == PieceColor::White ? 8 : 1)) {
        // Check if move is promotion, and if it is, display promotion menu.
        /*PieceType toPromote;
        while (true) {
            sf::Event event;
            if (window.pollEvent(event)) {
                switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    exit(0);
                default:
                    break;
                }
            }
            window.clear(moving_piece->getColor() == PieceColor::White ? sf::Color::White : sf::Color(100, 100, 100));
            sf::Transformable transformer;
            sf::RenderStates specificState;
            transformer.setPosition(100, 350);
            for (PieceType i : { PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight }) {
                transformer.move(100, 0);
                specificState.transform = transformer.getTransform();
                window.draw(this->pieces_sprites[static_cast<int>(moving_piece->getColor()) * 6 + static_cast<int>(i)], specificState);
            }
            window.display();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2f MousePosF = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2i MousePos(MousePosF.x, MousePosF.y);
                if (MousePos.y >= 350 && MousePos.y <= 450 && MousePos.x >= 200 && MousePos.x <= 600) {
                    switch ((MousePos.x - 200) / 100) {
                    case 0:
                        toPromote = PieceType::Queen;
                        break;
                    case 1:
                        toPromote = PieceType::Rook;
                        break;
                    case 2:
                        toPromote = PieceType::Bishop;
                        break;
                    case 3:
                        toPromote = PieceType::Knight;
                        break;
                    case 4:
                        throw std::out_of_range("Out of promotable Pieces");
                    }
                    break;
                }
            }
        }
        moving_piece->promotePawnTo(toPromote);*/
    } else if (moving_piece->getType() == PieceType::Pawn
        && abs(source.getRow() - destiny.getRow()) == 2) {
        // If pawn double moved, store moved pawn.
        this->pawn_double_moved_last_turn = moving_piece;
    }
    // Move piece
    accessBoard(destiny.getCol(), destiny.getRow()) = moving_piece;

    // Change turn color
    changeTurnColor();
}