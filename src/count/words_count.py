import keyboard

word_count = 0

def count_words(event):
    global word_count
    if event.name == "space":
        word_count += 1

keyboard.on_press(count_words)
keyboard.wait()