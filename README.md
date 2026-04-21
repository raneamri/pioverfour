# $$\frac{\pi}{4}$$

pioverfour is a simple simulation of the phenomenon described in [this](https://arxiv.org/abs/2602.14487) paper.

"Toss a coin until the first time the observed proportion of heads exceeds $\frac{1}{2}$
(that is, until the cumulative number of heads exceeds the cumulative number of tails)
and record that fraction; for instance, if the tosses proceeded as tails, heads, tails,
heads, heads you would record the fraction $\frac{3}{5}$.
Now perform a second run of coin-tosses, starting afresh and again recording a fraction that
exceeds $\frac{1}{2}$ (starting afresh means that you ignore all earlier tosses). Repeat. As you perform
more and more such trials, the average of the fractions will approach $\frac{\pi}{4}$."

Colloquially, an instance of this coin toss game ends when we count one more heads than tails. Jim Propp
suggests in his paper that if the termination criteria was instead $\textit{two}$ more heads than tails, the
average of the fraction converges to $\ln 2$. Let the surplus of heads over tails required for the game to
terminated be $s$. At $s = 1$, we see convergence to $\frac{\pi}{4}$ and for $s = 2$ to $\ln 2$. This simulation
allows to generalise for any $s$. For $s = 3$, the simulation hints at convergence towards $\frac{\pi^2}{6} - 1$. Curiously, I can't seem to guess the pattern here, but it seems that as $s$ approaches infinity, the ratio approaches $\frac{1}{2}$.

A similar research to the one stated above can be found [here](https://math.mit.edu/~apost/courses/18.204-2016/18.204_Jon_Lu_final_paper.pdf). In this research, Jon Lu states his interest in a repeat analysis with a biased coin.
The simulation described in this program uses a `bernoulli_distribution` to simulate coin tosses and therefore allows
for such bias in our distribution to be parametrised, potentially contributing to further research on this topic.

The quantity calculated by the simulation is

$$
\mathbb{E}\left[\frac{H}{T_s}\right]
$$

Thanks to some very smart folks on MathsStackExchange and this simulation, it has been proved that the quantity this
simulation evaluates is the PhiLerch ($\Phi$) special function, specifically $\frac{s}{2}\Phi(1, -1, \frac{s}{2})$. Read more about it [here](https://math.stackexchange.com/questions/5132523/unexpected-pi-and-ln2-in-simple-coin-toss-stopping-rule).