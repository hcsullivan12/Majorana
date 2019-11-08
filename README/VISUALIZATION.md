## Event display
The Majorana event display allows one to view the reconstruction results in real time on data or monte carlo. To run the event display on simulation, run
```
python evd/SIM_EVD.py
```
Make sure the reconstruction parameters match your simulation parameters, e.g. number of sipms. Click the start button and voila!

$ ./simulate -c <path_to_config.ini> --vis ON/OFF (visualization flag, default is OFF)