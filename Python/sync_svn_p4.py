import os

# 0 '': null
# 1 filename : Add file or dir
# 2 filename : delete file or dir
def get_optype_filename(line):
	# may have 2 spaces
	line = line.replace('  ',' ')
	list = line.split(' ')
	if len(list) > 2:
		if cmp(list[0], 'Added:') == 0:
			return 1,list[1]
		elif cmp(list[0], 'Deleted:') == 0:
			return 2,list[1]
	return 0,' '

def p4_command(optype, filename):
	# while dir or file exists
	if os.path.exists(filename):
		# Add file
		if optype == 1:
			add_cmd = 'p4 add -d -f %s' % filename
			os.system(add_cmd)
		# Delete file
		elif optype == 2:
			# When delete a dir, delete all files in this dir
			if os.path.isdir(filename):
				for root,dirs,files in os.walk(filename):
					for filespath in files:
						d_filename = os.path.join(root,filespath)
						delete_cmd = 'p4 delete -v %s' % d_filename
						os.system(delete_cmd)
				# Delete the dir just by python, while p4 do not delete dir in locals
				# os.removedirs(filename)
				global need_remove_dirs
				need_remove_dirs.append(filename)
			else:
				delete_cmd = 'p4 delete -v %s' % filename	
				os.system(delete_cmd)	

def p4_config():
	os.system('p4 set P4CLIENT=nfs14')
	os.system('p4 set P4PASSWD=Love@808414')
	os.system('p4 set P4PORT=sha-p4:1666')
	os.system('p4 set P4USER=houwb')
	os.system('p4 login')

# Config
p4_config()	

filename_ops = {}
added = 0
deleted = 0
file = open('update_log.txt','r')
need_remove_dirs = []

# Parse log file
for line in file:
	optype, filename = get_optype_filename(line)
	if optype == 1 or optype == 2:
		if optype == 1:
			added = added + 1
			filename_ops[filename] = optype
		elif optype == 2:
			deleted = deleted + 1
			filename_ops[filename] = optype

# Do command
for k, v in filename_ops.iteritems():
	p4_command(v, k)

print 'added:%d' % added
print 'deleted:%d' % deleted

#for dir in need_remove_dirs:
#	os.removedirs(dir)

# os.system('p4 submit -f revertunchanged -d "sync by sync tool"')





