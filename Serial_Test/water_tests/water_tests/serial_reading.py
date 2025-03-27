import serial
import csv
import datetime
now = datetime.datetime.now()
date = now.strftime("%Y%m%d%H%M%S")

# Liste des colonnes dans le CSV
header = ["time", "PS61", "PS62", "PS63", "PS64", "FM61"]

# Changer le port COM si nécessaire (voir Arduino IDE)
arduino = serial.Serial(port="COM6", baudrate=115200, timeout=.1)

with open(f"cooling_channel_sensor_data_{date}.csv", "w") as file:
    writer = csv.writer(file)
    # Mettre les noms des colonnes en haut du CSV
    writer.writerow(header)

    while True:
        try:
            # Lit les valeurs renvoyées par l'ADC
            result = arduino.readline().decode().split(",")
            pressure = [int(result[0])]
            for i in range(1, 5):
                # Formule pour convertir la valeur de l'ADC en pression
                pressure.append(result[i])

            # Ecrire les nouvelles pressions dans le CSV
            writer.writerow(pressure)
            print(pressure)
        except KeyboardInterrupt:
            break
        except IndexError:
            pass
        except ValueError:
            pass