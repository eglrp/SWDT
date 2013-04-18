%----------------------------------------------------------
% example in Matlab

f = @(x) exp(-x.^2 / 2) .* (1 + (sin(3*x)).^2) .* (1 + (cos(5*x).^2));

num_samples = 10000;
burn_in_period = 1000;
thinning_period = 5;
smpl0 = slicesample(1, num_samples, 'pdf', f, 'thin', thinning_period, 'burnin', burn_in_period);

[binheight, bincenter] = hist(x, 100);
h1 = bar(bincenter, binheight, 'hist');
set(h1, 'facecolor', [0.8 0.8 1]);

hold on;
xd = get(gca, 'XLim');
area = quad(f, -5, 5);
binwidth = bincenter(2) - bincenter(1);
%xgrid = linspace(xd(1), xd(2), 1000);
%y = (num_samples * binwidth / area) * f(xgrid);
%plot(xgrid, y, 'r', 'LineWidth', 2)
h2 = ezplot(@(x) (num_samples * binwidth / area) * f(x), [xd(1), xd(2)]);
set(h2, 'color', [1 0 0], 'linewidth', 2);
hold off;

%----------------------------------------------------------
% normal distribution (1-dim)

% target funcation
f = @(x) normpdf(x);

num_samples = 10000;
burn_in_period = 1000;
thinning_period = 5;
smpl2 = slicesample(1, num_samples, 'pdf', f, 'thin', thinning_period, 'burnin', burn_in_period);

figure;
histfit(smpl2, 100);
h = get(gca, 'Children');
set(h(2), 'FaceColor', [0.8 0.8 1]);

%----------------------------------------------------------
% multivariate normal distribution (2-dim)

% [ref] https://code.google.com/p/pmtk/source/browse/trunk/pmtk/examples/mcmcExamples/mhMvn2d.m?r=296

%run('D:\work_center\sw_dev\matlab\rnd\src\probabilistic_graphical_model\pmtk\pmtk3-1nov12\genpathPMTK.m');
%addpath('D:\working_copy\swl_https\matlab\src\statistical_analysis');

% target distribution
mu = [1 1];
sigma = [1 0 ; 0 1];
f = @(x) mvnpdf(x, mu, sigma);

num_samples = 100000;
burn_in_period = 1000;
thinning_period = 5;
smpl3 = slicesample([1 1], num_samples, 'pdf', f, 'thin', thinning_period, 'burnin', burn_in_period);

xedges = linspace(-1, 3, 50);
yedges = linspace(-1, 3, 50); 
histmat = hist2(smpl3(:,1), smpl3(:,2), xedges, yedges);

figure;
pcolor(xedges, yedges, histmat');
colorbar;
axis square tight;

%----------------------------------------------------------
% gamma distribution (1-dim)

% target distribution
alpha = 2.43;
beta = 1;
f = @(x) gampdf(x, alpha, beta);

num_samples = 10000;
burn_in_period = 1000;
thinning_period = 5;
smpl4 = slicesample(1, num_samples, 'pdf', f, 'thin', thinning_period, 'burnin', burn_in_period);

figure;
if 0
	%xxhat = cumsum(smpl4.^2) ./ (1:num_samples)';
	%plot(1:num_samples, xxhat);
	subplot(2,1,1), hist(smpl4, 100);
	axis_val = axis;
	subplot(2,1,2), ezplot(f, [axis_val(1) axis_val(2)]);
else
	hold on;
	[binheight, bincenter] = hist(smpl4, 100);
	h1 = bar(bincenter, binheight, 'hist');
	set(h1, 'facecolor', [0.8 0.8 1]);
	axis_val = axis;
	area = quad(f, axis_val(1), axis_val(2));
	binwidth = bincenter(2) - bincenter(1);
	h2 = ezplot(@(x) (num_samples * binwidth / area) * f(x), [axis_val(1) axis_val(2)]);
	set(h2, 'color', [1 0 0], 'linewidth', 2);
	hold off;
end;

%----------------------------------------------------------
% von Mises distribution

% [ref] "A Bayesian Analysis of Directional Data Using the von Mises-Fisher Distribution", G. Nunez-Antonio and E. Gutierrez-Pena, CSSC, 2005.
% [ref] "Finding the Location of a Signal: A Bayesian Analysis", P. Guttorp and R. A. Lockhart, JASA, 1988.

%addpath('D:\working_copy\swl_https\matlab\src\directional_statistics');

% target distribution
R_n = 20;
theta_n = pi;
c = 5;
n = 100;
kappa0 = 1;
f = @(theta) exp(kappa0 * R_n * cos(theta - theta_n)) / besseli(0, kappa0)^(c + n);

num_samples = 10000;
burn_in_period = 1000;
thinning_period = 5;
smpl5 = slicesample(1, num_samples, 'pdf', f, 'thin', thinning_period, 'burnin', burn_in_period);
smpl5 = mod(smpl5, 2*pi);

figure;
if 0
	subplot(2,1,1), hist(smpl5, 100)
	axis_val = axis;
	axis([0 2*pi axis_val(3) axis_val(4)]);
	subplot(2,1,2), ezplot(f, [0 2*pi]);
	%subplot(2,1,2), ezplot(f, [axis_val(1) axis_val(2)]);
else
	hold on;
	[binheight, bincenter] = hist(smpl5, 50);
	h1 = bar(bincenter, binheight, 'hist');
	set(h1, 'facecolor', [0.8 0.8 1]);
	axis_val = axis;
	area = quad(f, 0, 2*pi);
	binwidth = bincenter(2) - bincenter(1);
	h2 = ezplot(@(x) (num_samples * binwidth / area) * f(x), [0 2*pi]);
	set(h2, 'color', [1 0 0], 'linewidth', 2);
	axis([0 2*pi axis_val(3) axis_val(4)]);
	hold off;
end;
