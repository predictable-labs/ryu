.. libneo4j-client documentation master file, created by
   sphinx-quickstart on Sun May  7 13:41:04 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: Contents:

   ./README.md
   ./neo4j-client-doxy
   ./api-index


libneo4j-omni: libneo4j-client for all Neo4j versions
=====================================================

`libneo4j-omni <https://github.com/majensen/libneo4j-omni>`_ is a fork
of `Chris Leishman's <https://github.com/cleishm>`_ brilliant
`libneo4j-client <https://github.com/cleishm/libneo4j-client>`_, that
supports the `Neo4j <https://neo4j.com>`_ graph database `Bolt
protocol <https://neo4j.com/docs/bolt/current/>`_ through version 5.x.

Both the ``libneo4j-client`` library and the ``neo4j-client``
command-line REPL may be built with this repo. 

Features
--------

libneo4j-omni includes all features of libneo4j-client
plus the following:

- Support for features of Neo4j Bolt protocol versions 3, 4, and 5, including
  - Transactions 
  - Database selection
  - Element IDs on graph entities
- Most `Structure-based data
  types <https://neo4j.com/docs/bolt/current/bolt/structure-semantics/>`_
  returned by the Neo4j server along with Node, Relationship, and
  Path, including
  - Date, Time, LocalTime, DateTime, LocalDateTime
  - Duration
  - Point2D, Point3D
- Meaningful rendering of supported data types in the REPL client
- Allows the user to the version negotiation handshake with ``--support``,
  e.g. ``neo4j-client --support 6.0,5.7-5.0 <host>``.


