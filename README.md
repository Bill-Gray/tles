# tles
Contains various TLEs (Two-Line Elements) computed by me.  I do this by computing
orbital elements using Find_Orb (http://www.projectpluto.com/find_orb.htm) and generating
ephemerides;  I then do a least-squares-fit of TLEs to those ephemerides,  using the
`eph2tle` program in my [artificial satellite code]( https://github.com/Bill-Gray/sat_code).

The purpose of this is to be able to identify certain high-flying artificial satellites
over which asteroid hunters routinely stumble : objects moving slowly enough that it
isn't immediately obvious if the object is a slow-moving artsat or a fast-moving
near-earth object.  If it's the latter,  asteroid hunters will be very interested.
If it's the former,  they would like to ignore it as quickly as possible.

Most of these objects have periods of two days or more,  and somewhat elliptical
orbits,  causing them to appear to move slowly (like asteroids) at apogee.  They are
either not tracked by [Space-Track](https://www.space-track.org) or are poorly tracked by them.

These TLEs are used by my `sat_id` program used for figuring out which satellite
corresponds to a given object found in an image by an asteroid hunter. (`sat_id` is
also in the `sat_code` repository).  However,  these TLEs can be used with any
software that uses TLEs.

Note that updates will occur as new artsats are found and new observations are made
of known artsats.

Maintainer
----------
Bill Gray
p&#x202e;&ocirc;&#xe7;.&ouml;tulp&#x165;c&eacute;j&ocirc;&#x159;p&#x40;ot&uacute;l&#x202c;m
