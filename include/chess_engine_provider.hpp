#pragma once

#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <deque>
#include "piece.hpp"

namespace bprocess = boost::process;
namespace asio = boost::asio;

class ChessEngineProvider {
public:
    ChessEngineProvider(std::string exe_path);
    ChessEngineProvider(std::string exe_path, int difficulty);
    ~ChessEngineProvider();

    bool is_evaluating();
    bool is_running();
    void start();
    void stop();

    std::string get_best_move();
    void start_evaluating(std::string fen_pos, int depth);
private:
    void send_command(const std::string& cmd);
    void write();
    void start_read();
    void handle_line(const std::string& line);

    bool f_running = false;
    bool f_evaluating = false;
    std::string f_best_move = "";

    asio::io_context f_io_context;
    bprocess::async_pipe f_input_pipe;
    bprocess::async_pipe f_output_pipe;
    bprocess::child f_process;

    asio::executor_work_guard<asio::io_context::executor_type> f_work_guard;

    asio::streambuf f_streambuf;
    std::deque<std::string> f_cmd_queue;

    std::thread f_io_thread;
};