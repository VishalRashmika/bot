from telegram import (ReplyKeyboardMarkup, ReplyKeyboardRemove, Update, InlineKeyboardButton, InlineKeyboardMarkup)

todo_keyb = [
        [InlineKeyboardButton("List", callback_data='list'),
         InlineKeyboardButton("Add", callback_data='add'),
         InlineKeyboardButton("Delete",callback_data='delete')]
    ]
todo_keyb_markup = InlineKeyboardMarkup(todo_keyb)

list_todo_keyb = [
        [InlineKeyboardButton("Add", callback_data='add'),
         InlineKeyboardButton("Delete",callback_data='delete')]
    ]
list_todo_keyb_markup = InlineKeyboardMarkup(list_todo_keyb)

def list_tasks(tasks):
    msg = ""
    counter = 1
    for task in tasks:
        msg += f"{counter}. {task}\n"
        counter += 1
    return msg