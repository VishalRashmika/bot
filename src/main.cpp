#include <iostream>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

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

int main() {
    string token(getenv("TOKEN"));
    printf("Token: %s\n", token.c_str());

    Bot bot(token);

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
      {"/reminders", "/delete"},
      {"/sample", "Off"},
      {"Back"},
      {"Info", "About", "Map", "Etc"}
    }, start_keyboard);

    ReplyKeyboardMarkup::Ptr todo_keyboard(new ReplyKeyboardMarkup);
    createKeyboard({
      {"/list"},
      {"/add", "/delete"},
      {"/reminders", "Off"},
      {"Back"},
      {"Info", "About", "Map", "Etc"}
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
        bot.getApi().sendMessage(message->chat->id, "Executed <list>:");
    });
    // /todo - /add
    bot.getEvents().onCommand("add", [&bot, &todo_keyboard](Message::Ptr message) {
        string msg = "Executed <add>:";
        bot.getApi().sendMessage(message->chat->id, msg);
    // /todo - /delete
    });    
    bot.getEvents().onCommand("delete", [&bot, &todo_keyboard](Message::Ptr message) {
        string msg = "Executed <delete>:";
        bot.getApi().sendMessage(message->chat->id, msg);
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
