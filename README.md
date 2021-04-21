# **Homework#5 by Dmitry Beresnev B20-02 v1.0**

## __About Hw05__

Given files implement draft of Wendex.Taxi system.
Classes are distributed in the corresponding files.
The main-class file is used for testing and simulating.
You can play with `test.cpp` and see what happens.

## __How to run code__

I recommend use IDE with CMake file.
But you also can open windows terminal and run command `g++ "test.cpp"`
You should have GNU C++ compiler installed.

All information about different approaches for testing you can find
in file `test.cpp` in format of comments.

## __Versions__

I recommend use version `C++20`, as I tested program on this.
However, it is most likely files will work correctly on earlier versions.
Files correctly works on `Windows 10 2004` and with `GNU MINGW` compiler.

## __Dependencies__

#### _test.cpp_

files: `WendexTaxi.cpp`

#### _WendexTaxi.cpp_

files: `Gateways.cpp`

libraries: `iostream`

#### _Gateways.cpp_

files: `DataBase.cpp`

libraries: `string`, `vector`, `ctime`'`

#### _DataBase.cpp_

files: `Schemes.cpp`

libraries: `fstream`, `vector`, `iostream`

#### _Schemes.cpp_

libraries: `string`, `vector`, `utility`


## __About DataBase__
This example use simple files-based database.
Here I want to say a couple of words about its structure.

Database located in `db` folder in the root.
There are four folders for corresponding types of objects:
`admins`, `cars`, `drivers`, `orders` and `passengers`.
There also is file `configs.txt`, where all ID's are stored.
ID's are used for accessing corresponding files in objects' folders.
For admins, passengers and drivers ID is login,
for orders and car - just number.
First row of `configs.txt` contains admins' IDs, second - cars' IDs,
third - drivers' , fourth - orders' and fifth - passengers',
separated by '*' (default separator for my implementation).

For easier understanding, `db` folder contains five
.txt files with `schemes` for corresponding objects.
Schemes shows, what data each line contains.
It can be useful for testing or checking results.

Back to objects' folders: each of them contains .txt files
for every object with login (for admins, drivers & passengers) or just
number (for orders & cars) as title.

To fast clear the database, you can run `clearDB.bat`,
if you, of course, use Windows OS.
Otherwise, my sincere condolences.

## __Excuses and curses__

I understand, that this example is far from ideal. Moreover,
as I have experience with writing backend on JS with MongoDB,
I can say this example is, to put it mildly, not so good.
However, I really did my best and spent a lot of time on it.

I wanted to implement working with filesystem more elegant, but
`filesystem` library does not want me to do this.
It prefers to kill my MinGW and Clion every time I try to use it :)
As a result we have `configs.txt` and mnogo drugih kostiley.

## __Additional information__

Telegram: `@flip_floppa` (please, write me if you have questions!)
 
Damn it, I forgot write something funny
