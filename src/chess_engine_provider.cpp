#include "chess_engine_provider.hpp"

ChessEngineProvider::ChessEngineProvider(std::string exe_path)
    :   f_io_context(),
        f_input_pipe(f_io_context),
        f_output_pipe(f_io_context),
        f_process(exe_path, 
                bprocess::std_in < f_input_pipe,
                bprocess::std_out > f_output_pipe,
                f_io_context),
        f_work_guard(asio::make_work_guard(f_io_context))
{
    start_read();
    f_io_thread = std::thread([this]() { f_io_context.run(); });

    send_command("ucinewgame");
    // send_command("setoption name Skill Level Maximum Error value 600");
    // send_command("setoption name Skill Level Probability value 128");
}

ChessEngineProvider::ChessEngineProvider(std::string exe_path, int difficulty)
:   f_io_context(),
        f_input_pipe(f_io_context),
        f_output_pipe(f_io_context),
        f_process(exe_path, 
                bprocess::std_in < f_input_pipe,
                bprocess::std_out > f_output_pipe,
                f_io_context),
        f_work_guard(asio::make_work_guard(f_io_context))
{
    start_read();
    f_io_thread = std::thread([this]() { f_io_context.run(); });

    send_command("ucinewgame");
    send_command("setoption name Skill Level value " + std::to_string(difficulty));
}

ChessEngineProvider::~ChessEngineProvider()
{
    if (f_process.running())
        send_command("quit");
        f_process.wait();
    
    f_input_pipe.close();
    f_work_guard.reset();

    if (f_io_thread.joinable())
        f_io_thread.join();
}

bool ChessEngineProvider::is_evaluating()
{
    return f_evaluating;
}

bool ChessEngineProvider::is_running()
{
    return f_running;
}

void ChessEngineProvider::start()
{
    f_running = true;
#ifdef DEBUG
    std::cout << "Engine started\n";
#endif
}

void ChessEngineProvider::stop()
{
    f_running = false;
#ifdef DEBUG
    std::cout << "Engine stopped\n";
#endif
}

std::string ChessEngineProvider::get_best_move()
{
    std::string res = f_best_move;
    f_best_move = "";
    return res;
}

void ChessEngineProvider::start_evaluating(std::string fen_pos, int depth)
{
    f_evaluating = true;
    f_best_move = "";

    send_command("position fen " + fen_pos);
    send_command("go depth " + std::to_string(depth));
}

void ChessEngineProvider::send_command(const std::string &cmd)
{
    asio::post(f_io_context, [this, cmd]() {
        bool writing_in_progress = !f_cmd_queue.empty();
        f_cmd_queue.push_back(cmd + '\n');
        if (!writing_in_progress)
            this->write();
    });
}

void ChessEngineProvider::write()
{
    asio::async_write(
        f_input_pipe, 
        asio::buffer(f_cmd_queue.front()),
        [this](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
                f_cmd_queue.pop_front();
                if (!f_cmd_queue.empty())
                    this->write();
            }
            else {
                std::cerr << "Write error: " << ec.message() << "\n";
            }
        });
}

void ChessEngineProvider::start_read()
{
     asio::async_read_until(f_output_pipe, f_streambuf, '\n',
        [this](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                std::istream is(&f_streambuf);
                std::string line;
                std::getline(is, line);

                std::cout << line << '\n';
                handle_line(line);

                start_read();
            } else if (ec != asio::error::eof) {
                std::cerr << "Read error: " << ec.message() << "\n";
            }
        });
}

void ChessEngineProvider::handle_line(const std::string &line)
{
    if (line.starts_with("bestmove")) {
        f_best_move = line.substr(9, 13);
        f_evaluating = false;
    }
}
