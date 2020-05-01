#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "board.hpp"
#include "option_parser.hpp"
#include "player.hpp"
#include "player/human_player.hpp"
#include "player/sample_computer_player.hpp"

int main(int argc, char* argv[])
{
	using namespace reversi;

	const std::unordered_map<std::string, std::function<std::unique_ptr<Player>(Side)>> player_types = {
	    {"human", [](Side side) { return std::make_unique<HumanPlayer>(side); }},
	    {"sample", [](Side side) { return std::make_unique<SampleComputerPlayer>(side); }},
		{"random", [](Side side) { return std::make_unique<RandomComputerPlayer>(side); }},
		{"cp1", [](Side side) { return std::make_unique<ComputerPlayer1>(side); }},
		{"cp2", [](Side side) { return std::make_unique<ComputerPlayer2>(side); }},
		{"monte1", [](Side side) { return std::make_unique<MonteCarloPlayer1>(side); }},
		{"monte2", [](Side side) { return std::make_unique<MonteCarloPlayer2>(side); }},
		{"bit1", [](Side side) { return std::make_unique<BitPlayer1>(side); }},
	};

	auto command_line_params = parseCommandLineOptions(argc, argv, player_types);

	std::array<std::unique_ptr<Player>, 2> players;
	players.at(static_cast<std::size_t>(Side::BLACK)) = std::move(command_line_params.black_player);
	players.at(static_cast<std::size_t>(Side::WHITE)) = std::move(command_line_params.white_player);

	int win_rate = 0, lose_rate = 0;
	double mx_time = 0;
	for(int n=0; n<1; n++){
	printf("%d\n", n);

	Board board;

	int pass = 0; //相互パスの判定
	
	for (Side turn = Side::BLACK;; turn = getOpponentSide(turn)) {
		std::cout << board << "\n" << std::endl;

		if (board.count(CellState::EMPTY) == 0) {
			// no empty cell
			break;
		}
		auto legal_moves = board.getAllLegalMoves(turn);
		if (legal_moves.empty()) {
		  // pass
		  pass++;
		  if(pass>=2){
		    printf("両者パスしたのでこの試合は終了です。\n");
		    break;
		  }
		  std::cout << "turn = " << turn << ", move = Pass\n" << std::endl;
		  continue;
		}

		auto& turn_player = *players.at(static_cast<std::size_t>(turn));

		std::chrono::system_clock::time_point  start, end; // 型は auto で可
		start = std::chrono::system_clock::now(); // 計測開始時間

		auto move = turn_player.thinkNextMove(board);

		end = std::chrono::system_clock::now();  // 計測終了時間
 		double elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
		mx_time = std::max(mx_time, elapsed);

		if (!board.isLegalMove(move, turn)) {
			std::cout << "turn = " << turn << ", illegal move = " << move << std::endl;
			std::exit(1);
		}
		std::cout << "turn = " << turn << ", move = " << move << "\n" << std::endl;

		board.placeDisk(move, turn);
		pass = 0; //ここでpassを0にして2連パス以外を通さないようにする
	}

	//勝敗の判定
	int count_w = board.count(CellState::WHITE);
	int count_b = board.count(CellState::BLACK);

	printf("白が %d 個、黒が %d 個なので",count_w,count_b);
	if(count_b > count_w){
		printf("黒の勝ちです。");
		win_rate++;
	}else if(count_b < count_w){
		printf("白の勝ちです。");
		lose_rate++;
	}else{
		printf("引き分けです。");
	}
	printf("\n");
	std::cout << board << "\n" << std::endl;
	}
	//printf("%lf ms\n", mx_time);
	//printf("win rate:%d, lose rate:%d\n", win_rate, lose_rate);
	return 0;
}
