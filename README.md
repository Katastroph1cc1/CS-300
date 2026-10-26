Problem Statement & Approach
The goal of these projects was to build an advisement tool for ABC University that loads course data from a CSV file, stores it efficiently, prints an alphanumeric course list, and looks up individual course details and prerequisites.

I evaluated vectors, hash tables, and binary search trees for the task. Data structures directly drive performance and memory usage. Vectors made linear storage simple, but searching and maintaining sorted order added overhead. Hash tables offered fast lookups, but they don't keep data sorted. I chose the Binary Search Tree (BST) because it natively keeps courses sorted via in-order traversal while maintaining efficient search performance.

Overcoming Technical Roadblocks
I hit a few roadblocks during Project Two with CSV string parsing, stream buffer clearing, and handling cin and getline input loops cleanly. The program was hanging or misreading input during navigation. I resolved this by stepping through the stream state, properly handling newlines, and breaking down raw string lines into clean course fields through dedicated helper logic.

Software Architecture & Maintainability Growth
Working through these projects reinforced the importance of planning algorithms on paper before writing code. Analyzing runtimes and pseudocode upfront saved a lot of trial-and-error during implementation.

It also changed how I structure code for maintenance. By keeping the C++ source file clean, using modular functions, and focusing only on necessary documentation, the program stays readable and easy to modify. Using a BST abstracts the core data logic, making the code adaptable if course requirements or features expand down the road.
