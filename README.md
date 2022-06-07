# Differntiator
This program reads a mathematical expression, differentiates it and outputs the result in the format of tex document.

## Usage
Write in the file "diff_in.txt" the mathematical expression of one variable 'x', which you want to differentiate. After that run differentiator:
````sh
./tree
````
The program will differentiate the expression and record it, try to simplify it and display the response in the format of a tex file. You can find it in `/dump` folder.

## Example of usage
Example of differentiation of the expression:
````sh
((1+x)/(x^2))
````
After you run the program it will output:
![Differentiator output](/dump/example.jpeg)

## Expression representation
The differentiator represents the expression as an binary tree. Here's an example of the expression view above:
![Expression tree](/dump/example_tree.jpeg)
