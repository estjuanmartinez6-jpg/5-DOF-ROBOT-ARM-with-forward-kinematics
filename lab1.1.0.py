import serial
import time
import tkinter as tk
import threading

# Configuración del puerto serial
puerto = "COM6"  # Cambia según sea necesario
baudrate = 115200

try:
    ser = serial.Serial(puerto, baudrate, timeout=0)
    time.sleep(2)  # Esperar estabilización
except Exception as e:
    print(f"Error al abrir el puerto serial: {e}")
    ser = None

# Función para enviar datos continuamente
def enviar_datos_continuo():
    while ejecutando:
        if ser and ser.is_open:
            # Obtener los valores de los sliders
            valores = [slider.get() for slider in sliders]
            
            # Asignar las variables para los caracteres
            trama = '$'
            
            for valor in valores:
                # Descomprimir el valor
                if valor == 0:
                    # Si el valor es 0, enviar "000"
                    d1, d2, d3 = '0', '0', '0'
                elif valor < 10:
                    # Si el valor es menor a 10, enviar "00X" donde X es el valor
                    d1, d2, d3 = '0', '0', chr(valor + 48)
                elif valor >= 10 and valor < 100:
                    # Si el valor es entre 10 y 99, enviar "0XY" donde X es la decena y Y es la unidad
                    d1, d2, d3 = '0', chr(valor // 10 + 48), chr(valor % 10 + 48)
                else:
                    # Si el valor es entre 100 y 999, enviar "XYZ" donde X es la centena, Y la decena, y Z la unidad
                    d1, d2, d3 = chr(valor // 100 + 48), chr((valor % 100) // 10 + 48), chr(valor % 10 + 48)
                
                # Construir la parte de la trama para este slider
                trama += f"{d1}{d2}{d3}"

            # Eliminar la última coma y agregar el final de la trama
            trama =  '#\n' + trama
            
            # Mostrar la trama para depuración
            #print(f"Enviando: {trama.strip()}")  # Muestra la trama en la consola

            # Enviar la trama por serial
            ser.write(trama.encode())
            
            # Esperar antes de enviar nuevamente
            time.sleep(0.5)

# Crear la ventana principal
root = tk.Tk()
root.title("Control de Motores")
root.configure(bg="#2C3E50")  # Fondo oscuro

sliders = []
colores = ["#E74C3C", "#3498DB", "#2ECC71", "#F39C12", "#9B59B6", "#1ABC9C"]  # Colores para cada motor

# Encabezado
tk.Label(root, text="Motor", font=("Arial", 12, "bold"), bg="#2C3E50", fg="white").grid(row=0, column=0, padx=10, pady=5)
tk.Label(root, text="Ángulo", font=("Arial", 12, "bold"), bg="#2C3E50", fg="white").grid(row=0, column=1, padx=10, pady=5)

# Crear sliders en dos secciones
for i in range(3):
    tk.Label(root, text=f"Motor {i+1}", font=("Arial", 10, "bold"), bg=colores[i], fg="white", width=10).grid(row=i+1, column=0, padx=5, pady=5)
    slider = tk.Scale(root, from_=0, to=180, orient=tk.HORIZONTAL, length=200, bg=colores[i])
    slider.grid(row=i+1, column=1, padx=5, pady=5)
    sliders.append(slider)

for i in range(3):
    tk.Label(root, text=f"Motor {i+4}", font=("Arial", 10, "bold"), bg=colores[i+3], fg="white", width=10).grid(row=i+4, column=0, padx=5, pady=5)
    slider = tk.Scale(root, from_=0, to=180, orient=tk.HORIZONTAL, length=200, bg=colores[i+3])
    slider.grid(row=i+4, column=1, padx=5, pady=5)
    sliders.append(slider)

# Variable de control del hilo
ejecutando = True

# Iniciar el envío continuo en un hilo
hilo_envio = threading.Thread(target=enviar_datos_continuo, daemon=True)
hilo_envio.start()

# Función para cerrar el programa correctamente
def cerrar_programa():
    global ejecutando
    ejecutando = False
    if ser:
        ser.close()
    root.destroy()

# Botón para cerrar el programa
btn_cerrar = tk.Button(root, text="Cerrar", command=cerrar_programa, bg="#E74C3C", fg="white", font=("Arial", 12, "bold"))
btn_cerrar.grid(row=7, column=0, columnspan=2, pady=10)

# Ejecutar la interfaz gráfica
root.protocol("WM_DELETE_WINDOW", cerrar_programa)
root.mainloop()