import logging
import os
from telegram import (ReplyKeyboardMarkup, ReplyKeyboardRemove, Update, InlineKeyboardButton, InlineKeyboardMarkup)
from telegram.ext import (Application, CallbackQueryHandler, CommandHandler, ContextTypes, ConversationHandler, MessageHandler, filters)
import utils
import db

logging.basicConfig(
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s", level=logging.INFO
)
logging.getLogger("httpx").setLevel(logging.WARNING)
logger = logging.getLogger(__name__)

ACTION, LIST, ADD, DELETE, TASK = range(5)

async def send_photo(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await context.bot.send_photo(chat_id=update.effective_chat.id, photo=open('./dragon.png', 'rb'))
    

async def todo(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text("Please choose an action...",reply_markup=utils.todo_keyb_markup)
    return ACTION

async def action_choose(update: Update, context: ContextTypes.DEFAULT_TYPE):
    query = update.callback_query
    await query.answer()

    if query.data == 'list':
        msg = utils.list_tasks(db.tasks)
        await query.edit_message_text(f"Task List: \n{msg}",reply_markup=utils.list_todo_keyb_markup)
        # return LIST
    elif query.data == 'add':
        await query.edit_message_text("Enter the task to add: ")
        return ADD
    
    elif query.data == 'delete':
        msg = utils.list_tasks(db.tasks) + "\nEnter the task index to delete : "
        await query.edit_message_text(msg)
        return DELETE

async def option_add(update: Update, context: ContextTypes.DEFAULT_TYPE):
    user_task = update.message.text
    db.tasks.append(user_task)
    await update.message.reply_text(f"Task : {user_task} : Added Successfully!!!",reply_markup=utils.todo_keyb_markup)
    # return ConversationHandler.END
    return ACTION

async def option_delete(update: Update, context: ContextTypes.DEFAULT_TYPE):
    task_index = int(update.message.text)
    db.tasks.pop(task_index - 1)
    await update.message.reply_text(f"Task {task_index} deleted successfully!!!")
    msg =  "New tasks list:\n" + utils.list_tasks(db.tasks)
    await update.message.reply_text(msg,reply_markup=utils.todo_keyb_markup)
    # return ConversationHandler.END
    return ACTION

async def cancel(update: Update, context: ContextTypes.DEFAULT_TYPE) -> int:
    """Cancels and ends the conversation."""
    user = update.message.from_user
    logger.info("User %s canceled the conversation.", user.first_name)
    await update.message.reply_text(
        "Bye! I hope we can talk again some day.", reply_markup=ReplyKeyboardRemove()
    )
    return ConversationHandler.END

def main():
    BOT_TOKEN = os.environ['TOKEN']
    bot = Application.builder().token(BOT_TOKEN).build()

    todo_conversation_hander = ConversationHandler(
        entry_points=[CommandHandler('todo', todo)],
        states={
            ACTION: [CallbackQueryHandler(action_choose)],
            # LIST: [MessageHandler(filters.TEXT & ~filters.COMMAND, option_list)],
            ADD: [MessageHandler(filters.TEXT & ~filters.COMMAND, option_add)],
            DELETE: [MessageHandler(filters.TEXT & ~filters.COMMAND, option_delete)],
        },
        fallbacks=[CommandHandler("cancel", cancel)],
    )
    bot.add_handler(todo_conversation_hander)
    bot.add_handler(CommandHandler("photo", send_photo))
    bot.run_polling(allowed_updates=Update.ALL_TYPES)


if __name__ == '__main__':
    main()
