
import subprocess
import os

# get this directory
current_dir = os.getcwd()

# get top majorana directory
top_dir = os.environ.get('MAJORANADIR')
if top_dir is None:
    print('Could not find environment variable \'MAJORANADIR\'. Please point \'MAJORANADIR\' to the top directory.')
    exit()

print('')
print('Opening event display...')
print('')

evd_cmd = 'python SIPM_WHEEL_EVD.py'
proc = subprocess.Popen([evd_cmd], shell=True, stdin=None, stdout=None, stderr=None, close_fds=True)

print('')
print('Running G4 sim...')
print('')

# start a g4 session
g4_cmd = 'cd '+top_dir+'/build/ && ./simulate -c ../config/Configuration.ini -E ON'
os.system(g4_cmd)