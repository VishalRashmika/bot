#include <iostream>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>

#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

std::vector<std::string> todoList;
vector<string> bot_commands = {"start", "todo", "reminders", "list", "add", "delete"};
bool test_text_state = false;
bool task_delete_state = false;

void createOneColumnKeyboard(const vector<string>& buttonStrings, ReplyKeyboardMarkup::Ptr& kb)
{
  for (size_t i = 0; i < buttonStrings.size(); ++i) {
    vector<KeyboardButton::Ptr> row;
    KeyboardButton::Ptr button(new KeyboardButton);
    button->text = buttonStrings[i];
    row.push_back(button);
    kb->keyboard.push_back(row);
  }
}

void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb)
{
  for (size_t i = 0; i < buttonLayout.size(); ++i) {
    vector<KeyboardButton::Ptr> row;
    for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
      KeyboardButton::Ptr button(new KeyboardButton);
      button->text = buttonLayout[i][j];
      row.push_back(button);
    }
    kb->keyboard.push_back(row);
  }
}



std::string ListTodo(std::vector<std::string> List){
    int size = List.size();
    std::string message;
    std::string buffer;
    std::string count;
    for (int i = 0; i < size; i++){
        count = std::to_string(i + 1);
        buffer = "";
        buffer = count + ". " + List[i] + "\n";
        message.append(buffer);
    }
    return message;
}

void AddTodo(std::string task){
    todoList.push_back(task);
}

void DeleteTodo(int index){
    int location = index + 1;
    todoList.erase(todoList.begin() + index - 1);
}

int main() {
    todoList.push_back("Task1");
    todoList.push_back("Task2");
    todoList.push_back("Task3");
    todoList.push_back("Task4");
    todoList.push_back("Task5");

    string token(getenv("TOKEN"));
    printf("Token: %s\n", token.c_str());

    Bot bot(token);
    TgLongPoll long_poll(bot);

    // bot commands
    vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray(new BotCommand);
    cmdArray->command = "start";
    cmdArray->description = "Start the bot";
    commands.push_back(cmdArray);
    cmdArray = BotCommand::Ptr(new BotCommand);
    cmdArray->command = "todo";
    cmdArray->description = "Todo list";
    commands.push_back(cmdArray);
    cmdArray = BotCommand::Ptr(new BotCommand);
    cmdArray->command = "reminders";
    cmdArray->description = "Bot reminders";
    commands.push_back(cmdArray);

    bot.getApi().setMyCommands(commands);

    // keyboards
    // ReplyKeyboardMarkup::Ptr start_keyboard(new ReplyKeyboardMarkup);
    // createOneColumnKeyboard({"/todo", "/reminders", "/<sample>"}, start_keyboard);
    ReplyKeyboardMarkup::Ptr start_keyboard(new ReplyKeyboardMarkup);
    createKeyboard({
      {"/todo"},
      {"/reminders", "/del"},
      {"/sam", "Off"},
      {"Ba"},
      {"In", "Aut", "Mp", "Ec"}
    }, start_keyboard);

    ReplyKeyboardMarkup::Ptr todo_keyboard(new ReplyKeyboardMarkup);
    createKeyboard({
      {"/list"},
      {"/add", "/delete"},
      {"/reminders", "Off"},
      {"/delete"},
      {"Idw", "About", "Map", "Etc"}
    }, todo_keyboard);


    vector<BotCommand::Ptr> vectCmd;
    vectCmd = bot.getApi().getMyCommands();

    for(std::vector<BotCommand::Ptr>::iterator it = vectCmd.begin(); it != vectCmd.end(); ++it) {
        printf("cmd: %s -> %s\r",(*it)->command.c_str(),(*it)->description.c_str());
    }

    // /start
    bot.getEvents().onCommand("start", [&bot, &start_keyboard](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!, Bot initilized...", nullptr, nullptr, start_keyboard);
    });

    // /todo    
    bot.getEvents().onCommand("todo", [&bot, &todo_keyboard](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "todo executed", nullptr, nullptr, todo_keyboard);
    });
    // /todo - /list
    bot.getEvents().onCommand("list", [&bot](Message::Ptr message) {
        std::string msg = ListTodo(todoList);
        bot.getApi().sendMessage(message->chat->id, msg);
    });
    // /todo - /add
    bot.getEvents().onCommand("add", [&bot, &todo_keyboard](Message::Ptr message) {
        string msg = "Enter the Task: ";
        bot.getApi().sendMessage(message->chat->id, msg);
        test_text_state = true;
    });
    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        if (test_text_state) {
            AddTodo(message->text);
            bot.getApi().sendMessage(message->chat->id, "Task added successfully!!!");
            test_text_state = false;
            return;
        }

        for (const auto& command : bot_commands) {
            if ("/" + command == message->text) {
                return;
            }
        }

        bot.getApi().sendMessage(message->chat->id, "unknown command");
    });
    // /todo - /delete
    bot.getEvents().onCommand("delete", [&bot, &todo_keyboard](Message::Ptr message) {
        string msg = "Enter the task number to delete: ";
        bot.getApi().sendMessage(message->chat->id, msg);
        task_delete_state = true;
    });
        bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        if (task_delete_state) {
            int task_index = std::stoi(message->text);
            DeleteTodo(task_index);
            bot.getApi().sendMessage(message->chat->id, "Task removed successfully!!!");
            task_delete_state = false;
            return;
        }

        for (const auto& command : bot_commands) {
            if ("/" + command == message->text) {
                return;
            }
        }

        bot.getApi().sendMessage(message->chat->id, "unknown command");
    });



    // /reminders
    bot.getEvents().onCommand("reminders", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Executed reminders:");
    });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
