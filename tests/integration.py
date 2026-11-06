import tempfile,pathlib,subprocess
with tempfile.TemporaryDirectory()as tmp:
    f=pathlib.Path(tmp)/'jobs';f.write_text('/usr/bin/true\n/usr/bin/printf\targument with spaces\n')
    r=subprocess.run(['./app','2',str(f)],text=True,capture_output=True,timeout=5);assert r.returncode==0 and r.stdout=='argument with spaces',r;assert 'job 1 exit 0'in r.stderr and 'job 2 exit 0'in r.stderr
    f.write_text('/usr/bin/false\n');r=subprocess.run(['./app','1',str(f)],capture_output=True,timeout=5);assert r.returncode==1
print('Concurrent jobs, literal arguments, status reporting, and failure propagation passed')
