#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <climits>
#include <string.h>

#include "sample_computer_player.hpp"

#define INF 1000000000

namespace reversi
{

CellPosition SampleComputerPlayer::thinkNextMove(const Board& board)
{
	// choose the first legal move
	return board.getAllLegalMoves(getSide()).front();
}

CellPosition RandomComputerPlayer::thinkNextMove(const Board& board)
{	
	// choose a random legal move
	std::vector<reversi::CellPosition> legalMoves = board.getAllLegalMoves(getSide());

	return legalMoves[abs(rnd())%(int)legalMoves.size()];
}

CellPosition ComputerPlayer1::thinkNextMove(const Board& board)
{	
	// search
	CellPosition selectedMove;
	int evaluation = -INF;
	std::vector<CellPosition> legalMoves = board.getAllLegalMoves(getSide());
	int n_legalMoves = (int)legalMoves.size();

	if(board.count(getOwnState(getSide())) + board.count(getOwnState(getOpponentSide(getSide()))) > 53){
		max_depth = 11;
	}else{
		max_depth = 5;
	}

	if(n_legalMoves == 1){
		selectedMove = legalMoves[0];
	}else{
		for(int i=0; i<n_legalMoves; i++){
			Board copied_board = board.copy();
			copied_board.placeDisk(legalMoves[i], getSide());
			int temp_eval = ab_search(copied_board, 1, evaluation, INF, 0, 0);
			if(temp_eval > evaluation){
				evaluation = temp_eval;
				selectedMove = legalMoves[i];
			}
		}
	}
	
	return selectedMove;
}

int ComputerPlayer1::ab_search(const Board& board, int depth, int alpha, int beta, int pass, int flag)
{
	int evaluation;
	Side placeSide;
	if((depth+pass)%2 == 1){
		placeSide = getOpponentSide(getSide());
		evaluation = INF;
	}else{
		placeSide = getSide();
		evaluation = -INF;
	}
	// depth limit
	if(depth == max_depth){
		evaluation = evaluate(board);
	}else{
		std::vector<reversi::CellPosition> legalMoves = board.getAllLegalMoves(placeSide);
		if(legalMoves.empty()){ // pass
			if(flag){
				evaluation = evaluate(board);
			}else{
				evaluation = ab_search(board, depth, alpha, beta, pass+1, 1);
			}
		}else{
			int n_legalMoves = (int)legalMoves.size();

			for(int i=0; i<n_legalMoves; i++){
				Board copied_board = board.copy();
				copied_board.placeDisk(legalMoves[i], placeSide);

				if((depth+pass)%2 == 1){
					evaluation = std::min(evaluation, ab_search(copied_board, depth+1, alpha, beta, pass, 0));
					beta = evaluation;
				}else{
					evaluation = std::max(evaluation, ab_search(copied_board, depth+1, alpha, beta, pass, 0));
					alpha = evaluation;
				}
				if(alpha >= beta) break; // cut		
			}
		}
	}
	return evaluation;
}

int ComputerPlayer1::evaluate(const Board& board)
{
	int count = 0;
	int score[8][8] = {
		{ 30, -12,   0,  -1,  -1,   0, -12,  30,},
		{-12, -30,  -3,  -3,  -3,  -3, -30, -12,},
		{  0,  -3,   0,  -1,  -1,   0,  -3,   0,},
		{ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,},
		{ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,},
		{  0,  -3,   0,  -1,  -1,   0,  -3,   0,},
		{-12, -30,  -3,  -3,  -3,  -3, -30, -12,},
		{ 30, -12,   0,  -1,  -1,   0, -12,  30,},
	};

	CellState myState = getOwnState(getSide()), oppState = getOwnState(getOpponentSide(getSide()));
	int myStone = board.count(myState), oppStone = board.count(oppState);

	if(max_depth > 6){
		return myStone;
	}
	
	if(myStone + oppStone < 50){
		for(int i=0; i<Board::HEIGHT; i++){
			for(int j=0; j<Board::WIDTH; j++){
				if(board.get({i,j}) == myState){
					count += score[i][j];
				}else if(board.get({i,j}) == oppState){
					count -= score[i][j];
				}
			}
		}
	}else{
		count += (myStone - oppStone)*8;
	}

	// int meCount = 0, oppCount = 0, x = 0, y = 0;
	// CellState firstState;
	// for(int i=0; i<4; i++){
	// 	x = (i&1)*7; y = ((i>>1)&1)*7;
	// 	firstState = board.get({y,x});
	// 	if(firstState == CellState::EMPTY) continue;

	// 	for(int j=1; j<4; j++){
	// 		x = (i&1)*7; y = ((i>>1)&1)*7;
	// 		int temp = 0;
	// 		while(temp < 8){
	// 			if(firstState != board.get({y,x})) break;

	// 			x -= (2*(i&1)-1)*(j&1); y -= (2*((i>>1)&1)-1)*((j>>1)&1);
	// 			temp++;
	// 			// consider counting twice
	// 			if(temp == 8){
	// 				if((i==1 && j==1) || (i==2 && j!=2) || (i==3)){
	// 					temp = 0;
	// 					break;
	// 				}
	// 			}
	// 		}
	// 		if(firstState == myState){
	// 			meCount += temp;
	// 		}else{
	// 			oppCount += temp;
	// 		}
	// 	}
	// }

	// count += meCount*8;
	// count -= oppCount*8;

	count += 5*(int)board.getAllLegalMoves(getSide()).size();
	count -= 5*(int)board.getAllLegalMoves(getOpponentSide(getSide())).size();

	return count;
}

CellPosition ComputerPlayer2::thinkNextMove(const Board& board)
{	
	// search
	CellPosition selectedMove;
	int evaluation = -INF;
	std::vector<reversi::CellPosition> legalMoves = board.getAllLegalMoves(getSide());
	int n_legalMoves = (int)legalMoves.size();

	if(board.count(getOwnState(getSide())) + board.count(getOwnState(getOpponentSide(getSide()))) > 52){
		max_depth = 11;
	}else{
		max_depth = 5;
	}

	if(n_legalMoves == 1){
		selectedMove = legalMoves[0];
	}else{
		for(int i=0; i<n_legalMoves; i++){
			Board copied_board = board.copy();
			copied_board.placeDisk(legalMoves[i], getSide());
			int temp_eval = ab_search(copied_board, 1, evaluation, INF);
			if(temp_eval > evaluation){
				evaluation = temp_eval;
				selectedMove = legalMoves[i];
			}
		}
	}
	return selectedMove;
}

int ComputerPlayer2::ab_search(const Board& board, int depth, int alpha, int beta)
{
	int evaluation;
	Side placeSide;
	if(depth%2 == 1){
		placeSide = getOpponentSide(getSide());
		evaluation = INF;
	}else{
		placeSide = getSide();
		evaluation = -INF;
	}
	// depth limit
	if(depth == max_depth){
		evaluation = evaluate(board);
	}else{
		std::vector<reversi::CellPosition> legalMoves = board.getAllLegalMoves(placeSide);
		if(legalMoves.empty()){ // pass
			evaluation = ab_search(board, depth+1, alpha, beta);
		}else{
			int n_legalMoves = (int)legalMoves.size();

			for(int i=0; i<n_legalMoves; i++){
				Board copied_board = board.copy();
				copied_board.placeDisk(legalMoves[i], placeSide);

				if(depth%2 == 1){
					evaluation = std::min(evaluation, ab_search(copied_board, depth+1, alpha, beta));
					beta = evaluation;
				}else{
					evaluation = std::max(evaluation, ab_search(copied_board, depth+1, alpha, beta));
					alpha = evaluation;
				}
				if(alpha >= beta) break; // cut		
			}
		}
	}
	return evaluation;
}

int ComputerPlayer2::evaluate(const Board& board)
{
	int count = 0, count_me = 0, count_opp = 0;
	int score[8][8] = {
		{120, -20,  20,   5,   5,  20, -20, 120,},
		{-20, -40,  -5,  -5,  -5,  -5, -40, -20,},
		{ 20,  -5,  15,   3,   3,  15,  -5,  20,},
		{  5,  -5,   3,   3,   3,   3,  -5,   5,},
		{  5,  -5,   3,   3,   3,   3,  -5,   5,},
		{ 20,  -5,  15,   3,   3,  15,  -5,  20,},
		{-20, -40,  -5,  -5,  -5,  -5, -40, -20,},
		{120, -20,  20,   5,   5,  20, -20, 120,},
	};

	CellState myState = getOwnState(getSide()), oppState = getOwnState(getOpponentSide(getSide()));

	for(int i=0; i<Board::HEIGHT; i++){
		for(int j=0; j<Board::WIDTH; j++){
			if(board.get({i,j}) == myState){
				count += score[i][j];
				count_me++;
			}else if(board.get({i,j}) == oppState){
				count -= score[i][j];
				count_opp++;
			}
		}
	}

	//count += 3*(int)board.getAllLegalMoves(getSide()).size();
	//count -= 3*(int)board.getAllLegalMoves(getOpponentSide(getSide())).size();

	if(max_depth > 6){
		count = count_me;
	}
	return count;
}

CellPosition MonteCarloPlayer1::thinkNextMove(const Board& board)
{
	int mem;
	float ucb, max_ucb, total = 0;
	std::vector<reversi::CellPosition> legalMoves = board.getAllLegalMoves(getSide());
	const float n_legalMoves = (float)legalMoves.size(); 
	std::vector<std::pair<float, float>> count;	

	for(int i=0; i<(int)n_legalMoves; i++){
		Board copied_board = board.copy();
		std::pair<float, float> new_count;
		copied_board.placeDisk(legalMoves[i], getSide());
		if(playout(copied_board)){
			new_count.first = 1;
		}else{
			new_count.first = 0;
		}
		new_count.second = 1;
		count.push_back(new_count);
		total++;
	}

	if(n_legalMoves == 1){
		return legalMoves[0];
	}
	while(total < max_total){
		mem = 0;
		max_ucb = -1;
		for(int i=0; i<(int)n_legalMoves; i++){
			ucb = count[i].first/count[i].second + (float)sqrt(2.0*log((double)total)/(double)count[i].second);
			if(ucb > max_ucb){
				max_ucb = ucb;
				mem = i;
			}
		}
		Board copied_board = board.copy();
		copied_board.placeDisk(legalMoves[mem], getSide());
		if(playout(copied_board)){
			count[mem].first++;
		}
		count[mem].second++;
		total++;
	}
	mem = 0;
	max_ucb = -1;
	for(int i=0; i<(int)n_legalMoves; i++){
		ucb = count[i].first/count[i].second + (float)sqrt(2.0*log((double)total)/(double)count[i].second);
		if(ucb > max_ucb){
			max_ucb = ucb;
			mem = i;
		}
	}
	return legalMoves[mem];
}

bool MonteCarloPlayer1::playout(Board& board)
{
	int pass = 0;
	for (Side turn = getOpponentSide(getSide());; turn = getOpponentSide(turn)) {
		if (board.count(CellState::EMPTY) == 0) {
			// no empty cell
			break;
		}
		auto legal_moves = board.getAllLegalMoves(turn);
		int n_legalMoves = (int)legal_moves.size();
		if (n_legalMoves == 0) {
		  // pass
		  pass++;
		  if(pass>=2){
		    break;
		  }
		  continue;
		}
		auto move = legal_moves[abs(rnd())%n_legalMoves];
		board.placeDisk(move, turn);
		pass = 0; //ここでpassを0にして2連パス以外を通さないようにする
	}

	int count_me = board.count(getOwnState(getSide()));
	int count_opp = board.count(getOwnState(getOpponentSide(getSide())));
	return ((count_me - count_opp > 0) ? true : false);
}

CellPosition MonteCarloPlayer2::thinkNextMove(const Board& board)
{
	int mem, max_count = -1;
	std::vector<reversi::CellPosition> legalMoves = board.getAllLegalMoves(getSide());
	const int n_legalMoves = (int)legalMoves.size(); 
	std::vector<int> count;	

	if(n_legalMoves == 1){
		return legalMoves[0];
	}

	for(int i=0; i<(int)n_legalMoves; i++){
		int temp = 0;
		for(int j=0; j<max_total; j++){
			Board copied_board = board.copy();
			copied_board.placeDisk(legalMoves[i], getSide());
			if(playout(copied_board)){
				temp++;
			}
		}
		count.push_back(temp);
	}

	for(int i=0; i<n_legalMoves; i++){
		if(count[i] > max_count){
			mem = i;
			max_count = count[i];
		}
	}
	return legalMoves[mem];
}

bool MonteCarloPlayer2::playout(Board& board)
{
	int pass = 0;
	for (Side turn = getOpponentSide(getSide());; turn = getOpponentSide(turn)) {
		if (board.count(CellState::EMPTY) == 0) {
			// no empty cell
			break;
		}
		auto legal_moves = board.getAllLegalMoves(turn);
		int n_legalMoves = (int)legal_moves.size();
		if (n_legalMoves == 0) {
		  // pass
		  pass++;
		  if(pass>=2){
		    break;
		  }
		  continue;
		}
		auto move = legal_moves[abs(rnd())%n_legalMoves];
		board.placeDisk(move, turn);
		pass = 0;
	}
	int count_me = board.count(getOwnState(getSide()));
	int count_opp = board.count(getOwnState(getOpponentSide(getSide())));
	return ((count_me - count_opp > 0) ? true : false);
}

CellPosition BitPlayer1::thinkNextMove(const Board& board)
{	
	// search
	CellPosition selectedMove;
	int evaluation = -INF;
	unsigned long move, flipStone, myBoard = 0, oppBoard = 0;

	for(int i=7; i>=0; i--){
		for(int j=7; j>=0; j--){
			if(board.get({j, i}) == getOwnState(getSide())){
				myBoard ^= 1;
			}else if(board.get({j, i}) == getOwnState(getOpponentSide(getSide()))){
				oppBoard ^= 1;
			}
			if(i!=0 || j!= 0){
				myBoard <<= 1;
				oppBoard <<= 1;
			}
		}
	}

	unsigned long legalMoves = getAllLegalMoves(myBoard, oppBoard);
	int n_legalMoves = bitcount(legalMoves);

	if(bitcount(myBoard) + bitcount(oppBoard) > 50){
		max_depth = 13;
		evaluation = 32;
	}else{
		max_depth = 7;
	}
	selectedMove = (CellPosition){(ffsll(legalMoves)-1)%8, (ffsll(legalMoves)-1)/8};
	if(n_legalMoves != 1){
		while(legalMoves){
			int ffsll_n = ffsll(legalMoves)-1;
			move = (unsigned long)1 << ffsll_n;
			flipStone = flippingStone(myBoard, oppBoard, move);
			int temp_eval = ab_search(myBoard^(flipStone|move), oppBoard^flipStone, 1, evaluation, INF, 0, false);
			if(temp_eval > evaluation){
				evaluation = temp_eval;
				selectedMove = (CellPosition){ffsll_n%8, ffsll_n/8};
			}
			legalMoves ^= move;
		}
	}
	//std::cout << selectedMove.x << ' ' << selectedMove.y << '\n';
	return selectedMove;
}

int BitPlayer1::bitcount(unsigned long n)
{
	n = (n & 0x5555555555555555) + ((n & 0xAAAAAAAAAAAAAAAA) >> 1);
	n = (n & 0x3333333333333333) + ((n & 0xCCCCCCCCCCCCCCCC) >> 2);
	n = (n & 0x0F0F0F0F0F0F0F0F) + ((n & 0xF0F0F0F0F0F0F0F0) >> 4);
	n = (n & 0x00FF00FF00FF00FF) + ((n & 0xFF00FF00FF00FF00) >> 8);
	n = (n & 0x0000FFFF0000FFFF) + ((n & 0xFFFF0000FFFF0000) >>16);
	n = (n & 0x00000000FFFFFFFF) + ((n & 0xFFFFFFFF00000000) >>32);
	return (int)n;
}

unsigned long BitPlayer1::getAllLegalMoves(unsigned long myBoard, unsigned long oppBoard)
{
	unsigned long moves = 0, blankBoard = ~(myBoard|oppBoard);
	unsigned long rowChecker, colChecker, diagChecker, mask1, mask2;
	rowChecker = oppBoard & 0x7e7e7e7e7e7e7e7e;
	colChecker = oppBoard & 0x00ffffffffffff00;
    diagChecker = oppBoard & 0x007e7e7e7e7e7e00;

	std::vector<std::pair<int, unsigned long>> directionChecker;
	directionChecker.push_back({1, rowChecker});
	directionChecker.push_back({8,colChecker});
	directionChecker.push_back({7, diagChecker});
	directionChecker.push_back({9, diagChecker});

	for(int i=0; i<4; i++){
		int direction = directionChecker[i].first;
		unsigned long checker = directionChecker[i].second;
		mask1 = checker & (myBoard << direction);
        mask2 = checker & (myBoard >> direction);
		for(int j=0; j<5; j++){
			mask1 |= checker & (mask1 << direction);
			mask2 |= checker & (mask2 >> direction);
		}
        moves |= blankBoard & (mask1 << direction);
        moves |= blankBoard & (mask2 >> direction);
	}
	return moves;
}

unsigned long BitPlayer1::flippingStone(unsigned long myBoard, unsigned long oppBoard, unsigned long move)
{
	unsigned long flipStone = 0;
	unsigned long rowChecker, colChecker, diagChecker, mask1, mask2;
	rowChecker = oppBoard & 0x7e7e7e7e7e7e7e7e;
	colChecker = oppBoard & 0x00ffffffffffff00;
    diagChecker = oppBoard & 0x007e7e7e7e7e7e00;

	std::vector<std::pair<int, unsigned long>> directionChecker;
	directionChecker.push_back({1, rowChecker});
	directionChecker.push_back({8, colChecker});
	directionChecker.push_back({7, diagChecker});
	directionChecker.push_back({9, diagChecker});

	for(int i=0; i<4; i++){
		int direction = directionChecker[i].first;
		unsigned long checker = directionChecker[i].second;
		mask1 = checker & (move << direction);
        mask2 = checker & (move >> direction);
		for(int j=0; j<5; j++){
			mask1 |= checker & (mask1 << direction);
			mask2 |= checker & (mask2 >> direction);
		}
        if(myBoard & (mask1 << direction)){
			flipStone |= mask1;
		}
		if(myBoard & (mask2 >> direction)){
			flipStone |= mask2;
		}
	}

	return flipStone;
}

int BitPlayer1::ab_search(unsigned long myBoard, unsigned long oppBoard, int depth, int alpha, int beta, int pass, bool flag)
{
	int evaluation;
	
	// depth limit
	if(depth == max_depth){
		evaluation = evaluate(myBoard, oppBoard);
	}else{
		unsigned long legalMoves;
		if((depth+pass)%2 == 1){
			evaluation = INF;
			legalMoves = getAllLegalMoves(oppBoard, myBoard);
		}else{
			if(max_depth > 9){
				evaluation = 32;
			}else{
				evaluation = -INF;
			}
			legalMoves = getAllLegalMoves(myBoard, oppBoard);
		}
		if(legalMoves == 0){ // pass
			if(flag){
				evaluation = evaluate(myBoard, oppBoard);
			}else{
				evaluation = ab_search(myBoard, oppBoard, depth, alpha, beta, pass+1, true);
			}
		}else{
			while(legalMoves){
				int ffsll_n = (int)ffsll(legalMoves)-1;
				unsigned long move = (unsigned long)1 << ffsll_n;
				unsigned long flipStone;
				if((depth+pass)%2 == 1){
					flipStone = flippingStone(oppBoard, myBoard, move);
					evaluation = std::min(evaluation, ab_search(myBoard^flipStone, oppBoard^(move|flipStone), depth+1, alpha, beta, pass, false));
					beta = evaluation;
				}else{
					flipStone = flippingStone(myBoard, oppBoard, move);
					evaluation = std::max(evaluation, ab_search(myBoard^(move|flipStone), oppBoard^flipStone, depth+1, alpha, beta, pass, false));
					alpha = evaluation;
				}
				if(alpha >= beta) break; // cut	
				legalMoves ^= move;
			}
		}
	}
	return evaluation;
}

int BitPlayer1::evaluate(unsigned long myBoard, unsigned long oppBoard)
{

	if(max_depth > 9){
		return bitcount(myBoard);
	}

	int count = 0;
	int score[8][8] = {
		{ 30, -12,   0,  -1,  -1,   0, -12,  30,},
		{-12, -30,  -3,  -3,  -3,  -3, -30, -12,},
		{  0,  -3,   0,  -1,  -1,   0,  -3,   0,},
		{ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,},
		{ -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,},
		{  0,  -3,   0,  -1,  -1,   0,  -3,   0,},
		{-12, -30,  -3,  -3,  -3,  -3, -30, -12,},
		{ 30, -12,   0,  -1,  -1,   0, -12,  30,},
	};

	int myStone = bitcount(myBoard), oppStone = bitcount(oppBoard);
	
	if(myStone + oppStone < 50){
		for(int i=0; i<64; i++){
			if(myBoard & (1<<i)){
				count += score[i/8][i%8];
			}else if(oppBoard & (1<<i)){
				count -= score[i/8][i%8];
			}
		}
	}else{
		count += (myStone - oppStone)*8;
	}

	unsigned long myStableStone = 0, oppStableStone = 0;
	int directions[4] = {1, 8, 7, 9};
	unsigned long myMask1, myMask2, oppMask1, oppMask2;
	unsigned long checkers[4][2] = {{0x0100000000000001, 0x8000000000000080},
									{0x0000000000000081, 0x8100000000000000},
									{0x0000000000000080, 0x0100000000000000},
									{0x0000000000000001, 0x8000000000000000}};

	for(int i=0; i<4; i++){
		int direction = directions[i];
		myMask1 = myBoard & checkers[i][0];
		myMask2 = myBoard & checkers[i][1];
		oppMask1 = oppBoard & checkers[i][0];
		oppMask2 = oppBoard & checkers[i][1];
		myStableStone |= (myMask1 | myMask2);
		oppStableStone |= (oppMask1 | oppMask2);
		for(int j=0; j<6; j++){
			myMask1 = myBoard & (myMask1 << direction);
			myMask2 = myBoard & (myMask2 >> direction);
			oppMask1 = oppBoard & (oppMask1 << direction);
			oppMask2 = oppBoard & (oppMask2 >> direction);
			myStableStone |= (myMask1 | myMask2);
			oppStableStone |= (oppMask1 | oppMask2);
		}
	}
	
	count += 12*(bitcount(myStableStone) - bitcount(oppStableStone));

	if(myStone + oppStone < 15){
		count += 2*bitcount(getAllLegalMoves(myBoard, oppBoard));
		count -= 2*bitcount(getAllLegalMoves(oppBoard, myBoard));
	}else{
		count += 6*bitcount(getAllLegalMoves(myBoard, oppBoard));
		count -= 6*bitcount(getAllLegalMoves(oppBoard, myBoard));
	}

	return count;
}

}  // namespace reversi
