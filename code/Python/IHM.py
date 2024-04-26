import serial
import tkinter as tk
import time

#ser = serial.Serial('/dev/ttyUSB0', 115200)

# Création de la fenêtre principale
root = tk.Tk()
root.title("Menu Principal")

# Definition de certaines variables globales
z_position = -6
y_position = 3
x_position = 4
incr = 0.1
speed = 10
X_coord = 0
Y_coord = 0
Z_coord = 0

# Création des StringVar pour stocker les coordonnées
str_x_position = tk.StringVar()
str_y_position = tk.StringVar()
str_z_position = tk.StringVar()

# Initialisation des StringVar avec les valeurs initiales des coordonnées
str_x_position.set(str(x_position))
str_y_position.set(str(y_position))
str_z_position.set(str(z_position))

# Fonction pour déplacer le chariot selon l'axe X
def move_x(direction):
    global x_position
    global speed
    if direction == "forward":
        x_position += incr*speed
        #ser.write(b'X')
    elif direction == "backward":
        x_position -= incr*speed
        #ser.write(b'x')
    x_position = round(x_position,1)
    str_x_position.set(str(round(x_position,1)))
    print(f"X={x_position}")


# Fonction pour déplacer le chariot selon l'axe Y
def move_y(direction):
    global y_position
    global incr
    if direction == "forward":
        y_position += incr*speed
        #ser.write(b'Y')
    elif direction == "backward":
        y_position -= incr*speed
        #ser.write(b'y')
    y_position = round(y_position,1)
    str_y_position.set(str(round(y_position,1)))
    print(f"Y={y_position}")


# Fonction pour déplacer le chariot selon l'axe Z
def move_z(direction):
    global z_position
    global incr
    if direction == "up":
        z_position += incr*speed
        #ser.write(b'Z')
        print(f"Z={z_position}")
    elif direction == "down":
        z_position -= incr*speed
        #ser.write(b'z')
        print(f"Z={z_position}")
    z_position = round(z_position,1)
    str_z_position.set(str(round(z_position,1)))
    print(f"Z={z_position}")


def r0():
    while(z_position != 0):
        move_z('up')
    print('deplacement en Z fini')
    while(y_position != 0):
        move_y('backward')
    print('deplacement en Y fini')
    while(x_position != 0):
        move_x('backward')
    print('deplacement en X fini')

def goTo(X_coord, Y_coord, Z_coord):
    global x_position
    global y_position
    global z_position
    while (z_position != 0):
        move_z('up')
    print('deplacement en Z fini')
    time.sleep(1.5)
    while (y_position != 0):
        move_y('backward')
    print('deplacement en Y fini')
    time.sleep(1.5)
    if X_coord > x_position:
        while (x_position != X_coord):
            move_x('forward')
    else:
        while (x_position != X_coord):
            move_x('backward')
    print('deplacement en X fini')
    time.sleep(1.5)
    if Y_coord > y_position:
        while (y_position != Y_coord):
            move_y('forward')
    else:
        while (y_position != Y_coord):
            move_y('backward')
    print('deplacement en Y fini')
    time.sleep(1.5)
    if Z_coord > z_position:
        while (z_position != Z_coord):
            move_y('up')
    else:
        while (z_position != Z_coord):
            move_z('down')
    print('deplacement en Z fini')
    time.sleep(1.5)

def save_coords():
    global X_coord
    global Y_coord
    global Z_coord
    X_coord = float(entry_x_position.get())
    Y_coord = float(entry_y_position.get())
    Z_coord = float(entry_z_position.get())
    goTo(X_coord, Y_coord, Z_coord)
    

def showPosition():
    print(f'z={z_position}')
    print(f'y={y_position}')
    print(f'x={x_position}')


def speedRate(s):
    global speed
    if s == 1:
        #ser.write(b's1')
        speed = 1
    elif s == 2:
        #ser.write(b's2')
        speed = 10
    else:
        #ser.write(b's3')
        speed = 100

# Fonction pour afficher la page d'affichage des données
def open_data_display_window():
    data_window = tk.Toplevel()
    data_window.title("Affichage des données")

    # Labels pour afficher les données
    label_temperature = tk.Label(data_window, text="Température ambiante: ")
    label_temperature.grid(row=0, column=0, padx=10, pady=5)

    label_humidity = tk.Label(data_window, text="Humidité du sol: ")
    label_humidity.grid(row=1, column=0, padx=10, pady=5)

    label_pump_status = tk.Label(data_window, text="État de la pompe: ")
    label_pump_status.grid(row=2, column=0, padx=10, pady=5)

    label_fan_status = tk.Label(data_window, text="État des ventilateurs: ")
    label_fan_status.grid(row=3, column=0, padx=10, pady=5)

    label_light_status = tk.Label(data_window, text="État des lumières: ")
    label_light_status.grid(row=4, column=0, padx=10, pady=5)

    # Emplacements réservés pour afficher les valeurs des données
    entry_temperature = tk.Entry(data_window)
    entry_temperature.grid(row=0, column=1, padx=10, pady=5)

    entry_humidity = tk.Entry(data_window)
    entry_humidity.grid(row=1, column=1, padx=10, pady=5)

    entry_pump_status = tk.Entry(data_window)
    entry_pump_status.grid(row=2, column=1, padx=10, pady=5)

    entry_fan_status = tk.Entry(data_window)
    entry_fan_status.grid(row=3, column=1, padx=10, pady=5)

    entry_light_status = tk.Entry(data_window)
    entry_light_status.grid(row=4, column=1, padx=10, pady=5)


# Fonction pour afficher la page de contrôle des moteurs
def open_motor_control_window():
    global entry_x_position
    global entry_y_position
    global entry_z_position

    motor_window = tk.Toplevel()
    motor_window.title("Contrôle des moteurs")

    # Labels pour afficher la position du chariot
    label_x_position = tk.Label(motor_window, text="Trolley position X =")
    label_x_position.grid(row=0, column=0, padx=10, pady=5)

    label_y_position = tk.Label(motor_window, text="Trolley position Y =")
    label_y_position.grid(row=1, column=0, padx=10, pady=5)

    label_z_position = tk.Label(motor_window, text="Trolley position Z =")
    label_z_position.grid(row=2, column=0, padx=10, pady=5)

    # Emplacements réservés pour demander les valeurs des positions
    entry_x_position = tk.Entry(motor_window)
    entry_x_position.grid(row=0, column=1, padx=10, pady=5)

    entry_y_position = tk.Entry(motor_window)
    entry_y_position.grid(row=1, column=1, padx=10, pady=5)

    entry_z_position = tk.Entry(motor_window)
    entry_z_position.grid(row=2, column=1, padx=10, pady=5)

    # Création des boutons pour contrôler les moteurs
    btn_x_forward = tk.Button(motor_window, text="X+", command=lambda: move_x('forward'))
    btn_x_forward.grid(row=3, column=0, padx=10, pady=5)

    btn_x_backward = tk.Button(motor_window, text="X-", command=lambda: move_x('backward'))
    btn_x_backward.grid(row=3, column=1, padx=10, pady=5)

    btn_y_forward = tk.Button(motor_window, text="Y+", command=lambda: move_y('forward'))
    btn_y_forward.grid(row=4, column=0, padx=10, pady=5)

    btn_y_backward = tk.Button(motor_window, text="Y-", command=lambda: move_y('backward'))
    btn_y_backward.grid(row=4, column=1, padx=10, pady=5)

    btn_z_up = tk.Button(motor_window, text="Z+", command=lambda: move_z('up'))
    btn_z_up.grid(row=5, column=0, padx=10, pady=5)

    btn_z_down = tk.Button(motor_window, text="Z-", command=lambda: move_x('down'))
    btn_z_down.grid(row=5, column=1, padx=10, pady=5)

    btn_raz = tk.Button(motor_window, text="X0 Y0 Z0", command=lambda: r0())
    btn_raz.grid(row=6, column=0, padx=10, pady=5)

    btn_pos = tk.Button(motor_window, text="Xs Ys Zs", command=lambda: showPosition())
    btn_pos.grid(row=6, column=1, padx=10, pady=5)

    # Affichage de la position en X
    aff_x_position = tk.Label(motor_window, text='X = ')
    aff_x_position.grid(row=7, column=0, padx=5, pady=5)
    label_x_position_val = tk.Label(motor_window, textvariable=str_x_position)
    label_x_position_val.grid(row=7, column=1, padx=10, pady=5)

    # Affichage de la position en Y
    aff_y_position = tk.Label(motor_window, text='Y = ')
    aff_y_position.grid(row=8, column=0, padx=5, pady=5)
    label_y_position_val = tk.Label(motor_window, textvariable=str_y_position)
    label_y_position_val.grid(row=8, column=1, padx=10, pady=5)

    # Affichage de la position en Z
    aff_z_position = tk.Label(motor_window, text='Z = ')
    aff_z_position.grid(row=9, column=0, padx=5, pady=5)
    label_z_position_val = tk.Label(motor_window, textvariable=str_z_position)
    label_z_position_val.grid(row=9, column=1, padx=10, pady=5)

    # Bouton pour enregistrer les coordonnées demandées
    btn_save_coord_asked = tk.Button(motor_window, text="Go to", command=save_coords)
    btn_save_coord_asked.grid(row=1, column=2, columnspan=2, padx=10, pady=5)
    
    # Définition de la vitesse Vitesse 1
    speed1 = tk.Button(motor_window, text="Step = 0.1mm", command=lambda: speedRate(1))
    speed1.grid(row=10, column=0, columnspan=2, padx=10, pady=5)

    # Définition de la vitesse Vitesse 2
    speed2 = tk.Button(motor_window, text="Step = 1mm", command=lambda: speedRate(2))
    speed2.grid(row=10, column=1, columnspan=2, padx=10, pady=5)

    # Définition de la vitesse Vitesse 3
    speed3 = tk.Button(motor_window, text="Step = 10mm", command=lambda: speedRate(3))
    speed3.grid(row=10, column=2, columnspan=2, padx=10, pady=5)


# Configuration du fond d'écran du menu principal
background_image = tk.PhotoImage(file="menu_background.gif")
background_label = tk.Label(root, image=background_image)
background_label.place(relwidth=1, relheight=1)

# Création des boutons pour l'affichage des données et le contrôle des moteurs sur le menu principal
btn_data_display = tk.Button(root, text="Affichage des données", command=open_data_display_window)
btn_data_display.place(relx=0.3, rely=0.4, relwidth=0.2, relheight=0.2)

btn_motor_control = tk.Button(root, text="Contrôle des moteurs", command=open_motor_control_window)
btn_motor_control.place(relx=0.5, rely=0.4, relwidth=0.2, relheight=0.2)

# Boucle principale de l'interface graphique
root.mainloop()

