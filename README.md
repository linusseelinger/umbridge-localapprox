Local approximation microservice
===

UM-Bridge based microservice for local approximation as used in local aproximation MCMC.

Connects to an existing (Bayesian) posterior UM-Bridge model, itself provides fast local approximations to that posterior as an UM-Bridge model. To the UQ client, it just looks like a regular Bayesian posterior model again.

Currently assumes the underlying posterior to be running at http://localhost:4242, and itself provides the local approximation at http://localhost:4241.

Input: Sample parameter

Output: Local approximation to underlying posterior

TODO
===
* Investigate and treat tail correction etc.
* Parallel evaluation

Example setup
===

Run posterior model container:

```
docker run -it -p 4242:4243 linusseelinger/benchmark-analytic-gaussian-mixture
```

Run local approximation:

```
docker run -it --network=host linusseelinger/umbridge-localapprox
```

Run UQ client (based on PyMC):

```
python3 test.py http://localhost:4241
```