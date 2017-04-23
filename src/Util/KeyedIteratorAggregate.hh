<?hh // strict
// @todo remove this when this is a part of HHVM.

namespace GroovyCarrot\Event\Util;

use HH\KeyedTraversable;
use HH\KeyedIterator;

interface KeyedIteratorAggregate<+Tk, +Tv> extends KeyedTraversable<Tk, Tv>, \IteratorAggregate<Tv>
{
    public function getIterator(): KeyedIterator<Tk, Tv>;
}
