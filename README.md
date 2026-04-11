# $$\frac{\pi}{4}$$

pioverfour is a simple simulation of the phenomenon described in [this](https://arxiv.org/abs/2602.14487) paper.

"Toss a coin until the first time the observed proportion of heads exceeds $\frac{1}{2}$
(that is, until the cumulative number of heads exceeds the cumulative number of tails)
and record that fraction; for instance, if the tosses proceeded as tails, heads, tails,
heads, heads you would record the fraction $\frac{3}{5}$.
Now perform a second run of coin-tosses, starting afresh and again recording a fraction that
exceeds $\frac{1}{2}$ (starting afresh means that you ignore all earlier tosses). Repeat. As you perform
more and more such trials, the average of the fractions will approach $\frac{\pi}{4}$"

Colloquially, an instance of this coin toss game ends when we count one more heads than tails. Jim Propp
suggests in his paper that if the termination criteria was instead $\textit{two}$ more heads than tails, the
average of the fraction converges to $\ln 2$.

This simulation serves to check both convergences.