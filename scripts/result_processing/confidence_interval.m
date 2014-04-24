% Compute the confidence interval of the matrix mar. 
% ## INPUT VALUES
% 		matr is a 3D matrix. Going along the 3rd dimension, seed is changing. For each seed s, the matrix matr(:,:,s) is a table relating two variables (the variable related to the rows and the variable related to the columns).
% ## OUTPUT VALUES
%		avg is a table. The value avg(i,j) is the mean of the values mar(i,j,1), matr(i,j,2), matr(i,j,3), ....
%		ye is a table. The value ye(i,j) is the mean of the values mar(i,j,1), matr(i,j,2), matr(i,j,3), ....
function [avg, ye] = confidence_interval(matr)

	global severe_debug;
	
	if length(size(matr) ) == 3
		dim = 3; % The seeds are varying along the 3rd dimension
		avg = mean(matr, dim);
		opt = 0; % see http://www.gnu.org/software/octave/doc/interpreter/Descriptive-Statistics.html
		ye = ( 1.96/sqrt( size(matr,dim) ) ) .* std(matr, opt, dim);

	elseif length(size(matr) ) == 2
		% Only one seed is used
		avg = matr;
		ye = zeros(size(matr) );
	else
		matrix
		error("Dimension of matr is not correct")
	endif
end
