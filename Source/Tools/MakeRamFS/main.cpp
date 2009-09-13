#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct ramfs_header {
	unsigned int magic;	//For error checking
	unsigned int files;
};

struct ramfs_file_header {
	unsigned int name_length;
	unsigned int file_length;
};

#define INITRD_MAGIC 0x1337BEEF

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Error : no output file specified." << endl;
		cerr << "Usage : MakeRamFS <output.img> [<filename>:<ramfs filename> [...] ]" << endl;
		return 1;
	}

	ofstream output(argv[1], ios::out | ios::binary);

	ramfs_header hdr;
	hdr.magic = INITRD_MAGIC;
	hdr.files = argc - 2;

	output.write((char*)&hdr, sizeof(ramfs_header));

	for (int i = 2; i < argc; i++) {
		string name(argv[i]);
		string file;
		while (!name.empty()) {
			if (name[0] == ':') {
				name = name.substr(1, name.size() - 1);
				break;
			}
			file += name[0];
			name = name.substr(1, name.size() - 1);
		}

		ramfs_file_header fhdr;

		if (file == "") {	//This is a directory
			fhdr.name_length = name.size();
			fhdr.file_length = 0;	//File length of 0 means directory
			output.write((char*)&fhdr, sizeof(ramfs_file_header));
			output << name;
			continue;
		}
		
		ifstream infile(file.c_str(), ios::in | ios::binary);

		if (!infile) {
			fhdr.name_length = 0;	//Name and length = 0 means invalid file
			fhdr.file_length = 0;
			output.write((char*)&fhdr, sizeof(ramfs_file_header));
			continue;
		}

		fhdr.name_length = name.size();
		fhdr.file_length = 0;
	   	while (!infile.eof()) {
			char c;
			infile.read(&c, 1);
			fhdr.file_length++;
		}	

		infile.close(); infile.open(file.c_str(), ios::in | ios::binary);	//Rewind file

		output.write((char*)&fhdr, sizeof(ramfs_file_header));

		output << name;

		char *c = new char[fhdr.file_length];
		infile.read(c, fhdr.file_length);
		output.write(c, fhdr.file_length);
		delete [] c;

		infile.close();
	}

	output.close();

	return 0;
}
