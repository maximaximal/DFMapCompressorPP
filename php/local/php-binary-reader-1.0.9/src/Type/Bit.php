<?php

namespace PhpBinaryReader\Type;

use PhpBinaryReader\BinaryReader;
use PhpBinaryReader\BitMask;
use PhpBinaryReader\Exception\InvalidDataException;

class Bit implements TypeInterface
{
    /**
     * @var bool
     */
    private $signed = false;

    /**
     * Returns an unsigned integer from the bit level
     *
     * @param  \PhpBinaryReader\BinaryReader $br
     * @param  int                           $length
     * @throws \OutOfBoundsException
     * @throws InvalidDataException
     * @return int
     */
    public function read(BinaryReader &$br, $length)
    {
        if (!is_int($length)) {
            throw new InvalidDataException('The length parameter must be an integer');
        }

        $bitmask = new BitMask();
        $result = 0;
        $bits = $length;
        $shift = $br->getCurrentBit();

        if ($shift != 0) {
            $bitsLeft = 8 - $shift;

            if ($bitsLeft < $bits) {
                $bits -= $bitsLeft;
                $result = ($br->getNextByte() >> $shift) << $bits;
            } elseif ($bitsLeft > $bits) {
                $br->setCurrentBit($br->getCurrentBit() + $bits);

                return ($br->getNextByte() >> $shift) & $bitmask->getMask($bits, BitMask::MASK_LO);
            } else {
                $br->setCurrentBit(0);

                return $br->getNextByte() >> $shift;
            }
        }

        if (!$br->canReadBytes($length / 8)) {
            throw new \OutOfBoundsException('Cannot read bits, it exceeds the boundary of the file');
        }

        if ($bits >= 8) {
            $bytes = intval($bits / 8);

            if ($bytes == 1) {
                $bits -= 8;
                $result |= ($this->getSigned() ? $br->readInt8() : $br->readUInt8()) << $bits;
            } elseif ($bytes == 2) {
                $bits -= 16;
                $result |= ($this->getSigned() ? $br->readInt16() : $br->readUInt16()) << $bits;
            } elseif ($bytes == 4) {
                $bits -= 32;
                $result |= ($this->getSigned() ? $br->readInt32() : $br->readUInt32()) << $bits;
            } else {
                while ($bits > 8) {
                    $bits -= 8;
                    $result |= ($this->getSigned() ? $br->readInt8() : $br->readUInt8()) << 8;
                }
            }
        }

        if ($bits != 0) {
            $code = $this->getSigned() ? 'c' : 'C';
            $data = unpack($code, $br->readFromHandle(1));
            $br->setNextByte($data[1]);
            $result |= $br->getNextByte() & $bitmask->getMask($bits, BitMask::MASK_LO);
        }

        $br->setCurrentBit($bits);

        return $result;
    }

    /**
     * Returns a signed integer from the bit level
     *
     * @param  \PhpBinaryReader\BinaryReader $br
     * @param  int                           $length
     * @return int
     */
    public function readSigned(&$br, $length)
    {
        $this->setSigned(true);
        $value = $this->read($br, $length);
        $this->setSigned(false);

        return $value;
    }

    /**
     * @param boolean $signed
     */
    public function setSigned($signed)
    {
        $this->signed = $signed;
    }

    /**
     * @return boolean
     */
    public function getSigned()
    {
        return $this->signed;
    }
}
