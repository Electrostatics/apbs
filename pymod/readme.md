
Python Module for apbs/src
---

***kernel***: Smallest recognizable unit of computation

- Purposes:
    - port over the smallest possible kernels from the parallel directory
    - put hooks in all kernels in parallel directory to create small unit tests
    - Use tests cases to test new python implementations
    - ***heavily*** document any responsibilities that we find for any kernel/datastructure.

Once we have built up enough unit tests that we can compose them into larger units (say, to test a larger function in the parallel directory), we can then swap out implementations that may reduce the complexity of the underlying data structures and algorithms by several orders of magnitude.
The hope is that we can have some measure of the accuracy of our implementations before changing them.

Documentation
---

Each subdirectory of this directory should have the documenation relevant to the entire dir in the `__init__.py` file.
Docs relevant only to the files in the directory should live in those files.
