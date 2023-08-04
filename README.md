# ListDialog
 A Qt dialog that implements a list-form pattern


TITLE: ------------------------------------------

ListDialog class
Jenny Gray
August 2023

OVERVIEW: ---------------------------------------

This class demonstrates a user interface pattern I come across time and time again. It's for a modal dialog written C++ for Qt6 Widgets but the principle is easy to generalise.

The pattern is for creating and editing data of the following form:

Type description
   |
   Item description
      |
      Vector of double, attribute values

   Item description
      |
      Vector of double, attribute values, etc.

An example might be, you have a list of hardware tools and you want to define their names, weights, and prices. In this case you'd have

Type description: "Tool"
   |
   Item description: "Hammer"
      |
      Vector 2 (kg, weight), 20 (pounds, price)

   Item description: "Handsaw"
      |
      Vector 1 (kg, weight), 35 (pounds, price)

And so on. The best UI for this I've come across has:

1. A list control with the item descriptions on the left
2. A form containing editable data for the selected description on the right
3. Accept and Rest buttons on the form
4. Add and Delete buttons below the list
5. The usual OK and Cancel buttons.

PROJECT: ----------------------------------------

Supplied is 
1. A standard Qt6 Widgets CMakeLists.txt
2. listdlg.cpp and .h, the actual class
3. main*.* and testdlg.cpp and .h, showing trivially how to use ListDialog. TestDlg inherits ListDialog and implements rudimentary validation as a demonstration, which can be expanded.

To run it, open Qt Creator and open the CMakeLists.txt, then sort out a suitable configuration. I have it running with Desktop Qt 6.5.2 GCC 64bit, Debug and Release at time of writing but it's straightforward Qt6 code and just about anything should do, even if you have to start a new project and pull in the supplied C++ files.

USAGE: ------------------------------------------

The two non-trivial functions in MainWindow show the gist of it. Set up the Data object, create the dialog, connect the accepted slot, and away you go.

Note that the object is passed to and from the dialog by copy which can be optimised.





