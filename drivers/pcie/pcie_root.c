/****************************************************************************
 * nuttx/drivers/pcie/pcie_root.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/pcie/pcie.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

struct pcie_dev_type_s *pci_device_types[] =
{
  NULL,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pci_enumerate
 *
 * Description:
 *  Scan the PCI bus and enumerate the devices.
 *  Initialize any recognized devices, given in types.
 *
 * Input Parameters:
 *   bus    - PCI-E bus structure
 *   type   - List of pointers to devices types recognized, NULL terminated
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_enumerate(FAR struct pcie_bus_s *bus,
                  FAR struct pcie_dev_type_s **types)
{
  unsigned int bdf;
  uint16_t vid;
  uint16_t id;
  uint16_t rev;
  struct pcie_dev_s tmp_dev;
  struct pcie_dev_type_s tmp_type =
    {
      .name = "Unknown",
      .vendor = PCI_ID_ANY,
      .device = PCI_ID_ANY,
      .class_rev = PCI_ID_ANY,
      .probe = NULL,
    };

  if (!bus)
      return -EINVAL;
  if (!types)
      return -EINVAL;

  for (bdf = 0; bdf < CONFIG_PCIE_MAX_BDF; bdf++)
    {
      tmp_dev.bus = bus;
      tmp_dev.type = &tmp_type;
      tmp_dev.bdf = bdf;

      bus->ops->pci_cfg_read(&tmp_dev, PCI_CFG_VENDOR_ID, &vid, 2);
      bus->ops->pci_cfg_read(&tmp_dev, PCI_CFG_DEVICE_ID, &id, 2);
      bus->ops->pci_cfg_read(&tmp_dev, PCI_CFG_REVERSION, &rev, 2);

      if (vid == PCI_ID_ANY)
        continue;

      pciinfo("[%02x:%02x.%x] Found %04x:%04x, class/reversion %08x\n",
              bdf >> 8, (bdf >> 3) & 0x1f, bdf & 0x3,
              vid, id, rev);

      for (int i = 0; types[i] != NULL; i++)
        {
          if (types[i]->vendor == PCI_ID_ANY ||
              types[i]->vendor == vid)
            {
              if (types[i]->device == PCI_ID_ANY ||
                  types[i]->device == id)
                {
                  if (types[i]->class_rev == PCI_ID_ANY ||
                      types[i]->class_rev == rev)
                    {
                      if (types[i]->probe)
                        {
                          pciinfo("[%02x:%02x.%x] %s\n",
                                  bdf >> 8, (bdf >> 3) & 0x1f, bdf & 0x3,
                                  types[i]->name);
                          types[i]->probe(bus, types[i], bdf);
                        }
                      else
                        {
                          pcierr("[%02x:%02x.%x] Error: Invalid \
                                  device probe function\n",
                                  bdf >> 8, (bdf >> 3) & 0x1f, bdf & 0x3);
                        }
                      break;
                    }
                }
            }
        }
    }

  return OK;
}

/****************************************************************************
 * Name: pcie_initialize
 *
 * Description:
 *  Initialize the PCI-E bus and enumerate the devices with give devices
 *  type array
 *
 * Input Parameters:
 *   bus    - An PCIE bus
 *   types  - A array of PCIE device types
 *   num    - Number of device types
 *
 * Returned Value:
 *   OK if the driver was successfully register; A negated errno value is
 *   returned on any failure.
 *
 ****************************************************************************/

int pcie_initialize(FAR struct pcie_bus_s *bus)
{
  return pci_enumerate(bus, pci_device_types);
}

/****************************************************************************
 * Name: pci_enable_device
 *
 * Description:
 *  Enable device with MMIO
 *
 * Input Parameters:
 *   dev - device
 *
 * Return value:
 *   -EINVAL: error
 *   OK: OK
 *
 ****************************************************************************/

int pci_enable_device(FAR struct pcie_dev_s *dev)
{
  uint16_t old_cmd;
  uint16_t cmd;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_COMMAND, &old_cmd, 2);

  cmd = old_cmd | (PCI_CMD_MASTER | PCI_CMD_MEM);

  dev->bus->ops->pci_cfg_write(dev, PCI_CFG_COMMAND, &cmd, 2);

  pciinfo("%02x:%02x.%x, CMD: %x -> %x\n",
          dev->bdf >> 8, (dev->bdf >> 3) & 0x1f, dev->bdf & 0x3,
          old_cmd, cmd);

  return OK;
}

/****************************************************************************
 * Name: pci_find_cap
 *
 * Description:
 *  Search through the PCI-e device capability list to find given capability.
 *
 * Input Parameters:
 *   dev - Device
 *   cap - Bitmask of capability
 *
 * Returned Value:
 *   -1: Capability not supported
 *   other: the offset in PCI configuration space to the capability structure
 *
 ****************************************************************************/

int pci_find_cap(FAR struct pcie_dev_s *dev, uint16_t cap)
{
  uint8_t pos = PCI_CFG_CAP_PTR - 1;
  uint16_t status;
  uint8_t rcap;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_STATUS, &status, 2);

  if (!(status & PCI_STS_CAPS))
      return -EINVAL;

  while (1)
    {
      dev->bus->ops->pci_cfg_read(dev, pos + 1, &pos, 1);
      if (pos == 0)
          return -EINVAL;

      dev->bus->ops->pci_cfg_read(dev, pos, &rcap, 1);

      if (rcap == cap)
          return pos;
    }
}

/****************************************************************************
 * Name: pci_get_bar
 *
 * Description:
 *  Get a 32 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   ret    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_get_bar(FAR struct pcie_dev_s *dev, uint32_t bar,
                uint32_t *ret)
{
  if (bar > 5)
      return -EINVAL;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_BAR + bar * 4, ret, 4);

  return OK;
}

/****************************************************************************
 * Name: pci_get_bar64
 *
 * Description:
 *  Get a 64 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   ret    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_get_bar64(FAR struct pcie_dev_s *dev, uint32_t bar,
                  uint64_t *ret)
{
  if (bar > 4 || ((bar % 2) != 0))
      return -EINVAL;

  uint32_t barmem1;
  uint32_t barmem2;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_BAR + bar * 4, &barmem1, 4);
  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_BAR + bar * 4 + 4, &barmem2, 4);

  *ret = ((uint64_t)barmem2 << 32) | barmem1;

  return OK;
}

/****************************************************************************
 * Name: pci_set_bar
 *
 * Description:
 *  Set a 32 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   val    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_set_bar(FAR struct pcie_dev_s *dev, uint32_t bar,
                uint32_t val)
{
  if (bar > 5)
      return -EINVAL;

  dev->bus->ops->pci_cfg_write(dev, PCI_CFG_BAR + bar * 4, &val, 4);

  return OK;
}

/****************************************************************************
 * Name: pci_set_bar64
 *
 * Description:
 *  Set a 64 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   val    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_set_bar64(FAR struct pcie_dev_s *dev, uint32_t bar,
                  uint64_t val)
{
  if (bar > 4 || ((bar % 2) != 0))
      return -EINVAL;

  uint32_t barmem1 = (uint32_t)val;
  uint32_t barmem2 = (uint32_t)(val >> 32);

  dev->bus->ops->pci_cfg_write(dev, PCI_CFG_BAR + bar * 4, &barmem1, 4);
  dev->bus->ops->pci_cfg_write(dev, PCI_CFG_BAR + bar * 4 + 4, &barmem2, 4);

  return OK;
}

/****************************************************************************
 * Name: pci_map_bar
 *
 * Description:
 *  Map address in a 32 bits bar in the flat memory address space
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   length - Map length, multiple of PAGE_SIZE
 *   ret    - Bar Content if not NULL
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_map_bar(FAR struct pcie_dev_s *dev, uint32_t bar,
                unsigned long length, uint32_t *ret)
{
  if (bar > 5)
      return -EINVAL;

  uint32_t barmem;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_BAR + bar * 4, &barmem, 4);

  if (((bar % 2) == 0 &&
      (barmem & PCI_BAR_64BIT) == PCI_BAR_64BIT) ||
      (barmem & PCI_BAR_IO)    == PCI_BAR_IO)
      return -EINVAL;

  if (!dev->bus->ops->pci_map_bar)
      return -EINVAL;

  dev->bus->ops->pci_map_bar(dev, barmem, length);

  if (ret)
    *ret = barmem;

  return OK;
}

/****************************************************************************
 * Name: pci_map_bar64
 *
 * Description:
 *  Map address in a 64 bits bar in the flat memory address space
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   length - Map length, multiple of PAGE_SIZE
 *   ret    - Bar Content if not NULL
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_map_bar64(FAR struct pcie_dev_s *dev, uint32_t bar,
                  unsigned long length, uint64_t *ret)
{
  if (bar > 4 || ((bar % 2) != 0))
      return -EINVAL;

  uint32_t barmem1;
  uint32_t barmem2;
  uint64_t barmem;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_BAR + bar * 4, &barmem1, 4);

  if ((barmem1 & PCI_BAR_64BIT) != PCI_BAR_64BIT ||
      (barmem1 & PCI_BAR_IO)    == PCI_BAR_IO)
      return -EINVAL;

  dev->bus->ops->pci_cfg_read(dev, PCI_CFG_BAR + bar * 4 + 4, &barmem2, 4);

  barmem = ((uint64_t)barmem2 << 32) | barmem1;

  if (!dev->bus->ops->pci_map_bar64)
      return -EINVAL;

  dev->bus->ops->pci_map_bar64(dev, barmem, length);

  if (ret)
    *ret = barmem;

  return OK;
}
