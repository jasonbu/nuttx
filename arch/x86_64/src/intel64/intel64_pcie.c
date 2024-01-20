/****************************************************************************
 * arch/x86_64/src/intel64/intel64_pcie.c
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

#include <nuttx/pcie/pcie.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Functions Definitions
 ****************************************************************************/

static int intel64_pci_cfg_write(FAR struct pcie_dev_s *dev, uintptr_t addr,
                                 FAR const void *buffer, unsigned int size);

static int intel64_pci_cfg_read(FAR struct pcie_dev_s *dev, uintptr_t addr,
                                FAR void *buffer, unsigned int size);

static int intel64_pci_map_bar(FAR struct pcie_dev_s *dev, uint32_t addr,
                               unsigned long length);

static int intel64_pci_map_bar64(FAR struct pcie_dev_s *dev, uint64_t addr,
                                 unsigned long length);

static int intel64_pci_msix_register(FAR struct pcie_dev_s *dev,
                                     uint32_t vector, uint32_t index);

static int intel64_pci_msi_register(FAR struct pcie_dev_s *dev,
                                    uint16_t vector);

/****************************************************************************
 * Public Data
 ****************************************************************************/

static struct pcie_bus_ops_s g_intel64_pcie_bus_ops =
{
  .pci_cfg_write     = intel64_pci_cfg_write,
  .pci_cfg_read      = intel64_pci_cfg_read,
  .pci_map_bar       = intel64_pci_map_bar,
  .pci_map_bar64     = intel64_pci_map_bar64,
  .pci_msix_register = intel64_pci_msix_register,
  .pci_msi_register  = intel64_pci_msi_register,
};

static struct pcie_bus_s g_intel64_pcie_bus =
{
  .ops = &intel64__pcie_bus_ops,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: intel64_pci_cfg_write
 *
 * Description:
 *  Write 8, 16, 32, 64 bits data to PCI-E configuration space of device
 *  specified by dev
 *
 * Input Parameters:
 *   bdf    - Device private data
 *   buffer - A pointer to the read-only buffer of data to be written
 *   size   - The number of bytes to send from the buffer
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

static int intel64_pci_cfg_write(FAR struct pcie_dev_s *dev, uintptr_t addr,
                                 FAR const void *buffer, unsigned int size)
{

}

/****************************************************************************
 * Name: intel64_pci_cfg_read
 *
 * Description:
 *  Read 8, 16, 32, 64 bits data from PCI-E configuration space of device
 *  specified by dev
 *
 * Input Parameters:
 *   dev    - Device private data
 *   buffer - A pointer to a buffer to receive the data from the device
 *   size   - The requested number of bytes to be read
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

static int intel64_pci_cfg_read(FAR struct pcie_dev_s *dev, uintptr_t addr,
                                FAR void *buffer, unsigned int size)
{

}

/****************************************************************************
 * Name: intel64_pci_map_bar
 *
 * Description:
 *  Map address in a 32 bits bar in the memory address space
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   length - Map length, multiple of PAGE_SIZE
 *   ret    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

static int intel64_pci_map_bar(FAR struct pcie_dev_s *dev, uint32_t addr,
                               unsigned long length)
{

}

/****************************************************************************
 * Name: intel64_pci_map_bar64
 *
 * Description:
 *  Map address in a 64 bits bar in the memory address space
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   length - Map length, multiple of PAGE_SIZE
 *   ret    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

static int intel64_pci_map_bar64(FAR struct pcie_dev_s *dev, uint64_t addr,
                                 unsigned long length)
{

}

/****************************************************************************
 * Name: intel64_pci_msi_register
 *
 * Description:
 *  Map device MSI vectors to a platform IRQ vector
 *
 * Input Parameters:
 *   dev - Device
 *   vector - IRQ number of the platform
 *
 * Returned Value:
 *   <0: Mapping failed
 *    0: Mapping succeed
 *
 ****************************************************************************/

static int intel64_pci_msi_register(FAR struct pcie_dev_s *dev,
                                    uint16_t vector)
{

}

/****************************************************************************
 * Name: intel64_pci_msix_register
 *
 * Description:
 *  Map a device MSI-X vector to a platform IRQ vector
 *
 * Input Parameters:
 *   dev - Device
 *   vector - IRQ number of the platform
 *   index  - Device MSI-X vector number
 *
 * Returned Value:
 *   <0: Mapping failed
 *    0: Mapping succeed
 *
 ****************************************************************************/

static int intel64_pci_msix_register(FAR struct pcie_dev_s *dev,
                                     uint32_t vector, uint32_t index)
{

}
