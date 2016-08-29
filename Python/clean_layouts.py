####################################################
# clean unreferenced layouts
#
# ruleset and step:
# 1. find layouts referenced in Lua script
# 2. find layouts referenced in C++
# 3. build layouts dependency tree
# 4. find out unreferenced layouts
# 5. add unreferenced layouts to pending list
####################################################
import os
import stat
import sys
import re
import glob
import xml.etree.ElementTree as ElementTree
import subprocess

res_dir = '/app/res'
app_src_dir = '/app/src'
lua_dir = '/app/res/master/scripts'
layouts_dir = '/app/res/master/layouts'
texturepacks_ui_dir = '/app/res/master/texturepacks_ui'

class FileNode:
	file_cache = {}
	file_Tmp = {}
	
	@staticmethod
	def get_layoutpath_by_name(name):
		return os.path.normpath(sys.path[0] + layouts_dir + '/' + name + '.sx')	
	
	def __init__(self, path, cache = True):
		# full file path
		self.path = path
		# file base name with ext
		self.base_name = os.path.basename(path)		
		# file base name, exclude ext
		self.name = os.path.splitext(self.base_name)[0]
		# file content
		self.content = ''
		self.cache = cache
		if self.cache:
			self.cache_file()

	def cache_file(self):
		if self.path in FileNode.file_cache:
			self.content = FileNode.file_cache[self.path]
		else:
			try:
				f = open(self.path)				
				try:
					self.content = f.read()
					FileNode.file_cache[self.path] = self.content			
				finally:
					if f:
						f.close()
			except IOError:
				print '>>>> error, read %s failed' % self.path

	def contain_keywords(self, keywords, case_sensitive = True):
		if self.cache:
			if case_sensitive:
				pos = self.content.find(keywords)
				return pos != -1
			'''
			else:
				if self.path not in FileNode.file_Tmp:
					FileNode.file_Tmp[self.path] = self.content.lower()
				pos = FileNode.file_Tmp[self.path].find(keywords.lower())
				return pos != -1
			'''
		else:
			print '>>>> error, file %s not in cache' % self.base_name
			return False
		
	def get_path(self):
		return self.path
	
	def get_name(self):
		return self.name
		
	def get_content(self):
		return self.content

	def get_base_name(self):
		return self.base_name

class LayoutNode(FileNode):
	def __init__(self, path):
		FileNode.__init__(self, path)
		# node direct parent
		self.parent = None
		# node direct children
		self.children = []
		# is traverse
		self.traverse = False
		
	def set_parent(self, parent):
		self.parent = parent

	def set_traverse(self, traverse):
		self.traverse = traverse
		
	def get_traverse(self):
		return self.traverse

	def collect_children(self):
		# parse xml doc
		self.children = []
		root = ElementTree.fromstring(self.content)
		for element in root.iter('SubLayout'):	
			sub_element = element.find('layout')
			if sub_element != None:
				child_name = sub_element.text
				child_path = FileNode.get_layoutpath_by_name(child_name)

				if child_path in FileNode.file_cache:
					child_node = LayoutNode(child_path)
					child_node.set_parent(self)
					self.children.append(child_node)
				else:
					print '>>>> error, file %s not found in file cache' % child_name
		
	def get_unique_children(self):
		# may be contain same children node
		if len(self.children) < 1:
			return self.children
		else:
			s = []
			paths = {}
			for child in self.children:
				path = child.get_path()
				if path not in paths:
					paths[path] = 1
					s.append(child)
			return s
	
	def collect_images(self):
		image_list = set([])
		root = ElementTree.fromstring(self.content)
		# direct contain image		
		for element in root.iter('Image'):
			sub_element = element.find('image')
			if sub_element != None:
				image_name = sub_element.text
				image_list.add(image_name)
		# in SubLayout's subproperties
		for element in root.iter('SubLayout'):
			sub_properties = element.find('subproperties')
			if sub_properties != None:
				for image_properties in sub_properties.iter('Image'):
					image_name = image_properties.text
					image_list.add(image_name)

		return image_list

class CplusplusNode(FileNode):
	def __init__(self, path):
		FileNode.__init__(self, path)
		
class LuaNode(FileNode):
	def __init__(self, path):
		FileNode.__init__(self, path)

####################################################
## common function

#just curr dir
def find_files(find_dir, ext):
    for i in glob.glob(os.path.join(find_dir, ext)):
        yield i

# include sub dir
# func requires params: file_path, file_name
def foreach_files(search_dir, search_file_ext_list, func):
	if os.path.isdir(search_dir):
		for root, dirs, files in os.walk(search_dir):
			for filepath in files:
				full_file_name = os.path.join(root, filepath)
				path_pair = os.path.splitext(filepath)
				file_ext = path_pair[1]
				for ext in search_file_ext_list:
					if file_ext == ext:
						func(os.path.normpath(full_file_name))
						break

def add_to_list(node_list, key, value):
	node_list[key] = value
	
def get_relative_file_name(full_file_name, relative_path):
	pos = full_file_name.find(relative_path)
	if pos != -1:
		return full_file_name[pos + len(relative_path) + 1:len(full_file_name)]
	else:
		return ''	
	
def collect_all_cplusplus(cplusplus_file_list):
	search_dir = sys.path[0] + app_src_dir
	foreach_files(search_dir, {'.h', '.cpp'}, lambda file_path: add_to_list(cplusplus_file_list, file_path, CplusplusNode(file_path)))
	
def collect_all_lua(lua_file_list):
	search_dir = sys.path[0] + lua_dir
	foreach_files(search_dir, {'.lua'}, lambda file_path: add_to_list(lua_file_list, file_path, LuaNode(file_path)))
	
def p4_config():
	os.system('p4 set P4CLIENT=nfs14_firemonkey')
	os.system('p4 set P4PORT=sha-p4:1666')
	os.system('p4 set P4USER=houwb')
	os.system('p4 login')
	
def p4_command(optype, filename):
	# file exists
	if os.path.exists(filename):
		cmd = ''
		# add file	
		if optype == 1:
			cmd = 'p4 add -d -f %s' % filename
		# modify
		elif optype == 2:
			cmd = 'p4 edit %s' % filename			
		# delete file
		elif optype == 3:
			cmd = 'p4 delete -v %s' % filename
		# exec cmd
		if cmd != '':
			os.system(cmd)
####################################################
class LayoutsClean:	
	def __init__(self, cplusplus_file_list, lua_file_list, use_p4_command):
		self.cplusplus_file_list = cplusplus_file_list
		self.lua_file_list = lua_file_list
		self.use_p4_command = use_p4_command

		self.output_contain_log = False
		self.white_list_layouts = {}
		self.white_list_file = 'layout_white_list.txt'
		self.all_layouts_list = {}
		# the referenced_layouts_list may be not accurate, so it need white list
		self.referenced_layouts_list = {}
		self.unreferenced_layouts_list = {}

	def collect_white_list_layouts(self):
		try:
			f = open(self.white_list_file)	
			try:
				for name in f:
					name = name.strip('\n')
					file_path = FileNode.get_layoutpath_by_name(name)
					self.white_list_layouts[file_path] = LayoutNode(file_path)
			finally:
				if f:
					f.close()
		except IOError:
			print '>>>> error, read %s failed' % self.white_list_file			

	def collect_all_layouts(self):
		search_dir = sys.path[0] + layouts_dir
		foreach_files(search_dir, {'.sx'}, lambda file_path: add_to_list(self.all_layouts_list, file_path, LayoutNode(file_path)))

	def consturct_referenced_layouts_list(self):
		cplusplus_log_info = []
		lua_log_info = []
		
		for k, v in self.all_layouts_list.iteritems():
			file_name = v.get_name()
			file_path = v.get_path()
			keywords = '"' + file_name + '"'
			find = False
			
			if find == False:
				for k1, v1 in self.cplusplus_file_list.iteritems():
					if v1.contain_keywords(keywords):
						add_to_list(self.referenced_layouts_list, file_path, v)
						find = True
						log_text = "%s contain %s" % (v1.get_base_name(), keywords)
						cplusplus_log_info.append(log_text)
						break
			
			if find == False:
				for k2, v2 in self.lua_file_list.iteritems():
					if v2.contain_keywords(keywords):
						add_to_list(self.referenced_layouts_list, file_path, v)
						find = True
						log_text = "%s contain %s" % (v2.get_base_name(), keywords)					
						lua_log_info.append(log_text)
						break
			
			if find == False:
				if file_path in self.white_list_layouts:
					add_to_list(self.referenced_layouts_list, file_path, v)
					find = True	
	
		if self.output_contain_log:
			cplusplus_log_info.sort()
			lua_log_info.sort()
			
			log_file = open('layouts_contian.txt', 'w')
			for text in cplusplus_log_info:
				log_file.write(text + '\n')
			for text in lua_log_info:
				log_file.write(text + '\n')
			log_file.close()
			
	def consturct_referenced_sublayouts_list(self):
		traversed_list = {}
		need_traverse_list = []
	
		for k, v in self.referenced_layouts_list.iteritems():
			v.set_traverse(False)
			need_traverse_list.append(v)
		
		while len(need_traverse_list) > 0:
			node = need_traverse_list[0]
			node_path = node.get_path()
			# traverse it
			if node.get_traverse() == False:
				node.set_traverse(True)
				node.collect_children()
				children_list = node.get_unique_children()
				if len(children_list) > 0:
					for child in children_list:
						child_path = child.get_path()
						# add to referenced layouts list
						if child_path not in self.referenced_layouts_list:
							add_to_list(self.referenced_layouts_list, child_path, child)
						# wait for traverse
						if child.get_traverse() == False:
							need_traverse_list.append(child)
			# add it to traversed list		
			else:
				need_traverse_list.remove(node)
				if node_path not in traversed_list:
					traversed_list[node_path] = node
				
	def diff_for_unreferenced_layouts_list(self):
		for k, v in self.all_layouts_list.iteritems():
			if k not in self.referenced_layouts_list:
				self.unreferenced_layouts_list[k] = v
		
		log_file = open('layouts_unreferenced.txt', 'w')
		s = []
		for k, v in self.unreferenced_layouts_list.iteritems():
			s.append(v.get_name())
		s.sort(cmp=lambda x,y: cmp(x.lower(), y.lower()))
		for name in s:
			log_file.write(name + '\n')
		log_file.close()
		
	def get_referenced_layouts_list(self):
		return self.referenced_layouts_list
	
	def consturct_p4_pending_list(self):
		for k, v in self.unreferenced_layouts_list.iteritems():
			# make it readable
			os.chmod(k, stat.S_IREAD)
			p4_command(3, k)

	def clean_layouts(self):
		print 'b.0.>>> begine collect layouts white list'
		self.collect_white_list_layouts()
		print 'layouts white list num:%d' % len(self.white_list_layouts)
		
		print 'b.1.>>> begin collect all layouts'
		self.collect_all_layouts()
		print 'layouts num:%d' % len(self.all_layouts_list)
		
		print 'b.2.>>> begin collect referenced root layouts'
		self.consturct_referenced_layouts_list()
		print 'root layouts num:%d' % len(self.referenced_layouts_list)
		
		print 'b.3.>>> begin expand sublayouts'
		self.consturct_referenced_sublayouts_list()
		print 'referenced layouts num:%d' % len(self.referenced_layouts_list)
		
		print 'b.4.>>> begin find unreferenced layouts'
		self.diff_for_unreferenced_layouts_list()
		print 'unreferenced layouts num:%d' % len(self.unreferenced_layouts_list)
	
		if self.use_p4_command:
			print 'b.5.>>> begin consturct p4 pending list'
			self.consturct_p4_pending_list()		
####################################################
class ImageNode(FileNode):
	def __init__(self, path, pack_list_node):
		FileNode.__init__(self, path, False)
		self.relative_name = get_relative_file_name(self.get_path(), os.path.normpath('texturepacks_ui'))
		self.relative_name = self.relative_name.replace('\\', '/')
		self.pack_list_node = pack_list_node
	
	def get_relative_name(self):
		return self.relative_name
		
	def get_variant_file_name_list(self):
		# add ~2x, ~4x
		file_name_list = []
		file_name_list.append(self.path)

		file_dir = os.path.split(self.path)[0]
		file_ext = os.path.splitext(self.path)[1]
		
		file_2x = file_dir + '\\' + self.name + '~2x' + file_ext
		if os.path.exists(file_2x) == False:
			print '>>>> warning, file %s not exist' % file_2x
		else:
			file_name_list.append(file_2x)

		file_4x = file_dir + '\\' + self.name + '~4x' + file_ext
		if os.path.exists(file_4x) == False:
			print '>>>> warning, file %s not exist' % file_4x
		else:
			file_name_list.append(file_4x)
			
		return file_name_list
	
class PackListNode(FileNode):
	def __init__(self, path):
		FileNode.__init__(self, path)
		# image file node
		self.image_file_list = {}
		self.parse_files_pattern()
		
	def get_element_attribute(self, add_list, root, search_element, search_attribute):
		for element in root.iter(search_element):
			attribute = element.get(search_attribute)
			if attribute != None:
				add_list.append(attribute)
		
	def expand_path_pattern(self, path_pattern, file_list):
		search_path = sys.path[0] + texturepacks_ui_dir + '/' + path_pattern
		search_files = glob.glob(search_path)
		for search_file in search_files:
			file_list.append(search_file)
	
	def parse_files_pattern(self):
		root = ElementTree.fromstring(self.content)
		instances = root.find('Instances')
		if instances != None:
			# from Tools 'LoadPackList.cs'
			# 'SourcePath' or 'Include' or 'Image'
			source_paths_pattern = []
			source_files_list = []			
			self.get_element_attribute(source_paths_pattern, instances, 'SourcePath', 'pattern')
			self.get_element_attribute(source_paths_pattern, instances, 'Include', 'pattern')
			self.get_element_attribute(source_paths_pattern, instances, 'Image', 'pattern')
			for path_pattern in source_paths_pattern:
				self.expand_path_pattern(path_pattern, source_files_list)			
			# 'Exclude'
			exclude_paths_pattern = []
			exclude_files_list = []
			self.get_element_attribute(exclude_paths_pattern, instances, 'Exclude', 'pattern')
			for path_pattern in exclude_paths_pattern:
				self.expand_path_pattern(path_pattern, exclude_files_list)
			# 'Intersect'
			intersect_paths_pattern = []
			intersect_files_list = []
			self.get_element_attribute(intersect_paths_pattern, instances, 'Intersect', 'pattern')
			for path_pattern in intersect_files_list:
				self.expand_path_pattern(path_pattern, intersect_files_list)	
			# 'Override' just override some attribute, it doesn't change collection
			
			# so, get image file list
			find_files_list = source_files_list
			if len(exclude_files_list) > 0:
				find_files_list = [e for e in find_files_list if e not in exclude_files_list]
			if len(intersect_files_list) > 0:
				find_files_list = [e for e in find_files_list if e in intersect_files_list]
			
			# remove *~2x.{tif|png}, *~4x.{tif|png}
			re_pattern = re.compile('.*~((2x)|(4x))\.((tif)|(png))', re.I)
			
			for find_file in find_files_list:
				file_path = os.path.normpath(find_file)
				m = re_pattern.match(file_path)
				if m == None:			
					if file_path not in self.image_file_list:
						self.image_file_list[file_path] = ImageNode(file_path, self)
					else:
						print '>>>> warning, file %s repeated' % file_path

			print '%s:%d' % (self.get_base_name(), len(self.image_file_list))
	
	def get_image_file_list(self):
		return self.image_file_list

class TextureUIClean:
	def __init__(self, cplusplus_file_list, lua_file_list, referenced_layouts_list, use_p4_command):
		self.cplusplus_file_list = cplusplus_file_list
		self.lua_file_list = lua_file_list
		self.referenced_layouts_list = referenced_layouts_list
		self.use_p4_command = use_p4_command

		self.output_contain_log = False
		# pack list node
		self.pack_list = {}
		self.white_list = {}
		self.all_images = {}
		self.referenced_images = {}
		self.unreferenced_images = {}
		
	def collect_pack_list(self):
		for file_path in find_files(sys.path[0] + texturepacks_ui_dir, '*.packlist'):
			file_path = os.path.normpath(file_path)
			self.pack_list[file_path] = PackListNode(file_path)
		# white list
		white_list_path = sys.path[0] + texturepacks_ui_dir + '/white_list_packlist_for_tool.txt'
		white_list_path = os.path.normpath(white_list_path)
		self.white_list[white_list_path] = PackListNode(white_list_path)

	def collect_all_images(self):
		for k, v in self.pack_list.iteritems():
			image_list = v.get_image_file_list()
			for k2, v2 in image_list.iteritems():
				if k2 not in self.all_images:
					self.all_images[k2] = v2

	def collect_referenced_images(self):
		layout_images = set([])
		for k, v in self.referenced_layouts_list.iteritems():
			image_list = v.collect_images()
			for image in image_list:
				layout_images.add(image)
		
		# white list
		white_list_images = {}
		for k, v in self.white_list.iteritems():
			image_list = v.get_image_file_list()
			for k2, v2 in image_list.iteritems():
				if k2 not in white_list_images:
					white_list_images[v2.get_relative_name()] = v2

		white_list_log_info = []
		layout_log_info = []
		cplusplus_log_info = []
		lua_log_info = []

		for k, v in self.all_images.iteritems():
			relative_name = v.get_relative_name()
			find = False
			keywords = relative_name

			# in white list
			if find == False:
				if relative_name in white_list_images:
					add_to_list(self.referenced_images, relative_name, v)
					find = True
					log_text = "white list contain %s" % relative_name
					white_list_log_info.append(log_text)
			# in layouts
			if find == False:
				if relative_name in layout_images:
					add_to_list(self.referenced_images, relative_name, v)
					find = True
					log_text = "layout contain %s" % relative_name
					layout_log_info.append(log_text)
			# in lua
			if find == False:
				for k2, v2 in self.cplusplus_file_list.iteritems():
					if v2.contain_keywords(keywords):
						add_to_list(self.referenced_images, relative_name, v)
						find = True
						log_text = "%s contain %s" % (v2.get_base_name(), keywords)			
						cplusplus_log_info.append(log_text)
						break
			# in c++
			if find == False:
				for k3, v3 in self.lua_file_list.iteritems():
					if v3.contain_keywords(keywords):
						add_to_list(self.referenced_images, relative_name, v)
						find = True
						log_text = "%s contain %s" % (v3.get_base_name(), keywords)			
						lua_log_info.append(log_text)
						break
						
		if self.output_contain_log:
			white_list_log_info.sort()
			layout_log_info.sort()
			cplusplus_log_info.sort()
			lua_log_info.sort()
			
			log_file = open('textureui_contian.txt', 'w')
			for text in white_list_log_info:
				log_file.write(text + '\n')			
			for text in layout_log_info:
				log_file.write(text + '\n')		
			for text in cplusplus_log_info:
				log_file.write(text + '\n')
			for text in lua_log_info:
				log_file.write(text + '\n')
			log_file.close()

	def diff_for_unreferenced_images(self):
		for k, v in self.all_images.iteritems():
			relative_name = v.get_relative_name()
			if relative_name not in self.referenced_images:
				add_to_list(self.unreferenced_images, relative_name, v)

		log_file = open('textureui_unreferenced.txt', 'w')
		s = []
		for k, v in self.unreferenced_images.iteritems():
			s.append(k)
		s.sort(cmp=lambda x,y: cmp(x.lower(), y.lower()))
		for name in s:
			log_file.write(name + '\n')
		log_file.close()
		
	def consturct_p4_pending_list(self):
		if len(self.unreferenced_images) < 1:
			return
	
		for k, v in self.unreferenced_images.iteritems():
			variant_file_name_list = v.get_variant_file_name_list()
			for file_name in variant_file_name_list:
				# make it readable
				os.chmod(file_name, stat.S_IREAD)
				p4_command(3, file_name)

	def clean_texture_ui(self):
		print 'c.0.>>> begin collect pack list'
		self.collect_pack_list()
		print 'pack list num:%d' % len(self.pack_list)

		print 'c.1.>>> begin collect all images'			
		self.collect_all_images()
		print 'all image num:%d' % len(self.all_images)
		
		print 'c.2.>>> begin collect referenced images'	
		self.collect_referenced_images()
		print 'referenced image num:%d' % len(self.referenced_images)
		
		print 'c.3.>>> begin collect unreferenced_images'
		self.diff_for_unreferenced_images()
		print 'unreferenced image num:%d' % len(self.unreferenced_images)
		
		if self.use_p4_command:
			print 'c.4.>>> begin consturct p4 pending list'
			self.consturct_p4_pending_list()
			
			if len(self.unreferenced_images) > 0:
				print '6>>> begin repack textures'
				
				bat_path = os.path.normpath(sys.path[0] + '/scripts/WindowsCompressed/build_texturepacks.bat')
				print '%s' % bat_path
				p = subprocess.Popen("cmd.exe /c" + bat_path)
				p.wait()
	
				diff_dir_1x = sys.path[0] + res_dir + '/published.1x/texturepacks_ui/...'
				os.system('p4 edit ' + os.path.normpath(diff_dir_1x))
				os.system('p4 revert -a ' + os.path.normpath(diff_dir_1x))
				
				diff_dir_2x = sys.path[0] + res_dir + '/published.2x/texturepacks_ui/...'
				os.system('p4 edit ' + os.path.normpath(diff_dir_2x))
				os.system('p4 revert -a ' + os.path.normpath(diff_dir_2x))
				
				diff_dir_4x = sys.path[0] + res_dir + '/published.4x/texturepacks_ui/...'
				os.system('p4 edit ' + os.path.normpath(diff_dir_4x))
				os.system('p4 revert -a ' + os.path.normpath(diff_dir_4x))

if __name__ == '__main__':
	#argv
	use_p4_command = False
	if len(sys.argv) >= 2:
		if sys.argv[1] == 1:
			use_p4_command = True
	
	if use_p4_command:
		p4_config()

	print 'a.0.>>> begin collect .cpp|.h'
	cplusplus_file_list = {}
	collect_all_cplusplus(cplusplus_file_list)
	print '.cpp|.h num:%d' % len(cplusplus_file_list)
	
	print 'a.1.>>> begin collect .lua'
	lua_file_list = {}
	collect_all_lua(lua_file_list)
	print '.lua num:%d' % len(lua_file_list)

	layoutClean = LayoutsClean(cplusplus_file_list, lua_file_list, use_p4_command)
	layoutClean.clean_layouts()
	referenced_layouts_list = layoutClean.get_referenced_layouts_list()
	
	textureuiClean = TextureUIClean(cplusplus_file_list, lua_file_list, referenced_layouts_list, use_p4_command)
	textureuiClean.clean_texture_ui()

	if use_p4_command:
		print '!>>> print enter to quit'
		raw_input()
	