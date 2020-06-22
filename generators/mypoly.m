1;

function y = curve(x)
	c = 3/4;
	d = 0.5;

	r_0 = x < -1;
	r_1 = x>=-1 & x < 1;
	r_2 = x>= 1;
	y = zeros(size(x));
	y(r_1) = c*(x(r_1) - x(r_1).^3/3) + d;
	y(r_2) = 1;
end


function y = curve2(x, x_min, x_max)
	x_prime = 2*(x - x_min)/(x_max - x_min) + (-1);
	y = curve(x_prime, 1);
end

x = linspace(0, 1, 129);
y = curve2(x, 0.9, 1);
plot(x, y, 'linewidth', 1);
hold on
%plot(0.75, 0, '*')
hold off