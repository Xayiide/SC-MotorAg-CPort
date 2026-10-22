# SC-MotorAg-CPort

Port a C del simulador de motores y del algoritmo genético para crear controladores PID.
El código original lo proporcionó Norberto Cañas de Paz a los alumnos de Sistemas de Control y estaba escrito en Ada.

# Dependencias

El algoritmo no tiene dependencias, pero el script de Python sí. Todo está gestionado con `uv`, así que para instalar

# Modo de uso

La compilación y la ejecución se realiza con el script `ag.sh`. Para compilar el proyecto, utilizar `./ag.sh build`, y para ejecutarlo `./ag.sh run`. La compilación acepta la opción `verbose` para mostrar los comandos completos de `Cmake` y `traces` para mostrar los controladores generados internamente dentro del algoritmo genético. Esta opción imprime mucho texto, así que utilizar con cuidado.

Al terminar la ejecución, se escriben los resultados a un fichero de texto y se ejecuta un script de Python que los grafica.