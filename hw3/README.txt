
Using python2.
The parameters of the files in src/ are for task 1.

Steps:
	1. run "python split.py" to split the train.txt into 1.train~5.train & 1.test~5.test for 5-fold cross validation
	2. Use libmf to split source.txt and generate "model"
	3. run "python NMTF.py saveX" to combine the matrices in "model". It would generate "srcR".
	4. run "python NMTF.py init" to init the F, G, S, which are mentioned in [2] in report.pdf.
	5. run "python NMTF.py AL" to generate the "codebook".
	6. run "python CBT.py X" to do cross validation. X=1~5, the X-th validation.
	7. run "python CBT.py" to train all the train.txt and fill in the test.txt.

Caution:
	1. The matrix multiplication takes huge memory. For example, task1 took >
	   27G when it did "python NMTF.py AL".

